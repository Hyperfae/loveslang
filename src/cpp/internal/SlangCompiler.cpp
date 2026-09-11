#include "SlangCompiler.hpp"
#include "common/Exception.h"
#include "LOVESlangFilesystem.hpp"
#include "graphics/ShaderStage.h"
#include <array>
#include <iostream>
#include <memory>
#include <mutex>
#include <ranges>
#include <regex>
#include <slang-com-ptr.h>
#include <slang.h>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <vector>
namespace loveslang {

love::Type SlangCompiler::type("loveslang.SlangCompiler", &Object::type);

std::once_flag SlangCompiler::globalSessionCreated;
Slang::ComPtr<slang::IGlobalSession> SlangCompiler::globalSession;

SlangCompiler::SlangCompiler() {
    ensureGlobalSession();
    slang::SessionDesc sessionDesc = {};
    slang::TargetDesc targetDesc[1]{};
    targetDesc[0].format = SLANG_GLSL;
    sessionDesc.targets = targetDesc;
    sessionDesc.fileSystem = new LOVESlangFilesystem();
    sessionDesc.targetCount = 1;
    std::array<slang::PreprocessorMacroDesc, 1> preprocessorMacroDesc =
    {
        { "LOVESLANG", "LOVESLANG" },
    };
    sessionDesc.preprocessorMacros = preprocessorMacroDesc.data();
    sessionDesc.preprocessorMacroCount = preprocessorMacroDesc.size();
    
    std::vector<const char*> search_paths{ "__loveslang_lib_internal" };
    sessionDesc.searchPaths = search_paths.data();
    sessionDesc.searchPathCount = search_paths.size();

    globalSession->createSession(sessionDesc,
                                 session.writeRef());
}

SlangCompilerOutput SlangCompiler::compileToGLSL(std::string_view moduleName) {
    Slang::ComPtr<slang::IBlob> diagnosticsBlob;
    Slang::ComPtr<slang::IModule> slangModule;
    slangModule = session->loadModule(moduleName.data(), diagnosticsBlob.writeRef());
    if (!slangModule) {
        throw love::Exception("Slang compilation failure: %s", diagnosticsBlob != nullptr ? diagnosticsBlob->getBufferPointer() : "Unknown" );
    }
    return getCompilerOutputFromModule(slangModule);
}

static std::vector<Slang::ComPtr<slang::IEntryPoint>> getEntryPoints(Slang::ComPtr<slang::IModule> module) {
    auto entrypoint_count = module->getDefinedEntryPointCount();
    std::vector<Slang::ComPtr<slang::IEntryPoint>> entrypoints(love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM);
    // TODO: Find some way to infer these automatically
    static std::array<const char *, 3> entrypointNames {
        "vertexMain",
        "pixelMain",
        "computeMain"
    };
    
    for (int i = 0; i < love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM; i++) {
        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        module->findEntryPointByName(entrypointNames[i], entryPoint.writeRef());
        if (entryPoint) {
            entrypoints[i] = entryPoint;
        }
    }
    
    return entrypoints;
}

std::string SlangCompiler::postprocessStageCode(std::string_view inCode, love::graphics::ShaderStageType stage) {
    static std::array<const char *, love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> targetEntrypointNames {
        "vertexmain",
        "effect",
        "computemain"
    };
    
    std::string code{inCode};
    code = std::regex_replace(code, std::regex("^#version .*$", std::regex::multiline), std::string(""));
    code = std::regex_replace(code, std::regex("^layout\\(column_major\\) buffer;$", std::regex::multiline), std::string(""));
    code = std::regex_replace(code, std::regex("^layout\\(binding = .\\)$", std::regex::multiline), std::string(""));
    code = std::regex_replace(code, std::regex("void main"), std::string("void ") + targetEntrypointNames[stage]);
    // Fixup uniforms - currently required for LÖVE to accept our GLSL.
    code = std::regex_replace(code, std::regex("layout\\(std140\\) uniform block_GlobalParams_0(.|\\n|\\r)*globalParams_0;"), "uniform GlobalParams_0 globalParams_0;");

    return code;
}

static void assertSlangOK(SlangResult result, slang::IBlob* diagnosticsBlob) {
    if (result == SLANG_OK) return;
    if (!diagnosticsBlob)
        throw love::Exception("Slang failure (%#08x): Unknown", result);
    throw love::Exception("Slang failure (%#08x): %s", result, std::string_view((char*)diagnosticsBlob->getBufferPointer(), diagnosticsBlob->getBufferSize()));
}

SlangCompiler::StageInfo SlangCompiler::getRawStageCode(slang::IModule* slangModule, slang::IEntryPoint* entryPoint)
{
    // 5. Compose Modules + Entry Points
    std::array<slang::IComponentType*, 2> componentTypes =
        {
            slangModule,
            entryPoint
        };

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = session->createCompositeComponentType(
            componentTypes.data(),
            componentTypes.size(),
            composedProgram.writeRef(),
            diagnosticsBlob.writeRef());
        assertSlangOK(result, diagnosticsBlob);
    }
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = composedProgram->link(
            linkedProgram.writeRef(),
            diagnosticsBlob.writeRef());
        assertSlangOK(result, diagnosticsBlob);
    }
    Slang::ComPtr<slang::IBlob> outCode;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = linkedProgram->getEntryPointCode(
            0,
            0,
            outCode.writeRef(),
            diagnosticsBlob.writeRef());
        assertSlangOK(result, diagnosticsBlob);
    }
    static u_char stag{0};
    
    return {
        std::string((char*)outCode->getBufferPointer(), outCode->getBufferSize()),
        linkedProgram
    };
}

static std::vector<std::string_view> split_str_view(std::string_view view, std::string_view delim) {
    std::vector<std::string_view> output{};
    auto split = std::views::split(view, delim);
    for (auto line_raw : split) {
        output.push_back(std::string_view(line_raw));
    }
    return output;
}

static void parseResourceUniform(std::shared_ptr<UniformInfo>& info, std::vector<std::string_view>& lines, slang::VariableLayoutReflection* paramater, std::map<int, int>& binding_line_map)
{
    int binding_index = paramater->getBindingIndex();
    int current_line = binding_line_map[binding_index];
    if (paramater->getType()->getResourceShape() != SLANG_STRUCTURED_BUFFER) {
        std::smatch sm0;
        std::string line{lines[binding_line_map[binding_index] + 1]};
        std::regex_search(line, sm0, std::regex(("uniform (.*) (.*);")));
        info->glsl_names.push_back(sm0[2]);
        return;
    }
    switch (paramater->getType()->getResourceAccess()) {
    case SLANG_RESOURCE_ACCESS_CONSUME:
    case SLANG_RESOURCE_ACCESS_APPEND: {
        std::smatch sm0;
        std::string line{lines[binding_line_map[binding_index + 1]]};
        std::regex_search(line, sm0, std::regex(("buffer (.*) \\{")));
        info->glsl_names.push_back(sm0[1]);
        // TODO: Find counter buffer
    }
    case SLANG_RESOURCE_ACCESS_READ_WRITE: {
        std::smatch sm1;
        std::string line{lines[current_line]};
        std::regex_search(line, sm1, std::regex(("buffer (.*) \\{")));
        info->glsl_names.push_back(sm1[1]);
        break;
    }
    default:
        break;
    }
}

std::vector<UniformInfo> SlangCompiler::createUniformMap(StageInfo* stageinfo) {
    std::map<std::string, std::shared_ptr<UniformInfo>> map{};
    auto programLayout = stageinfo->linkedProgram->getLayout();
    int program_paramater_count = programLayout->getParameterCount();
    std::vector<std::string_view> split = split_str_view(std::string_view(stageinfo->glsl), std::string_view("\n"));
    std::map<int, int> binding_line_map{};
    for (int i = 0; i < split.size(); i++) {
        std::string_view line = split[i];
        std::cout << std::format("{} - {}", i + 1, line) << std::endl;
        if (!line.contains(std::string_view("binding ="))) {
            continue;
        } 
        std::smatch sm1;
        std::string linestring{line};
        std::regex_search(linestring, sm1, std::regex(("binding = (\\d+)")));
        binding_line_map.insert(std::pair(std::stoi(sm1[1]), i));
    }
    for (auto item : binding_line_map) {
        auto line = split[item.second];
        int i = item.second;
        for (int j = 0; j < program_paramater_count; j++) {
            slang::VariableLayoutReflection* paramater = programLayout->getParameterByIndex(j);
            if (paramater->getType()->getKind() == slang::TypeReflection::Kind::Resource) {
                auto slangname = std::string(paramater->getName());
                if (!map.contains(slangname)) {
                    std::shared_ptr<UniformInfo> info = std::make_shared<UniformInfo>();
                    info->slang_name = slangname;
                    map.insert({slangname, info});
                }
                auto info = map[slangname];
                uint bindingIndex = paramater->getBindingIndex();
                if (line.contains(std::format("binding = {}", bindingIndex))) {
                    std::cout << std::format("Found {} at {}, line {}", paramater->getName(), bindingIndex, i + 1) << std::endl;
                    switch (paramater->getType()->getResourceShape())
                    {
                    case SLANG_BYTE_ADDRESS_BUFFER:
                    case SLANG_RESOURCE_UNKNOWN:
                    case SLANG_ACCELERATION_STRUCTURE:
                    case SLANG_TEXTURE_SUBPASS:
                        break;
                    case SLANG_STRUCTURED_BUFFER:
                    default:
                        parseResourceUniform(info, split, paramater, binding_line_map);
                        break;
                    }
                }
            }
        }
    }
    std::vector<UniformInfo> list;
    for (auto& item : map) {
        list.push_back(*item.second);
    }
    return list;
}

SlangCompilerOutput SlangCompiler::getCompilerOutputFromModule(Slang::ComPtr<slang::IModule> module) {
    std::bitset<love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> stages{};
    auto entrypoinsByStage = getEntryPoints(module);
    std::stringstream final_code{};
    static std::array<const char *, love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> stageNames {
        "VERTEX",
        "PIXEL",
        "COMPUTE"
    };
    std::vector<SlangCompiler::StageInfo> stageCodes;
    for (int stage_id = 0; stage_id < love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM; stage_id++) {
        if (!entrypoinsByStage[stage_id])
            continue;
        stages[stage_id] = true;
        auto stage = (love::graphics::ShaderStageType)stage_id;
        final_code << "#ifdef " << stageNames[stage_id] << "\n";
        auto stage_info = getRawStageCode(module, entrypoinsByStage[stage_id].get());
        stage_info.stage = stage;
        stageCodes.push_back(stage_info);
        final_code << postprocessStageCode(stage_info.glsl, stage);
        final_code << "\n#endif\n";
    }
    auto uniformMap = std::make_shared<std::vector<UniformInfo>>();
    for (auto& stage : stageCodes) {
        auto linkedProgram = stage.linkedProgram;
        auto programLayout = linkedProgram->getLayout();
        
        std::stringstream thing;
        auto stageUniformMap = createUniformMap(&stage);
        thing << "BEGIN THING FOR \"" << love::graphics::ShaderStage::getConstant(stage.stage) << "\"\n";
        int count = stageUniformMap.size();
        for (int i = 0; i < count; i++) {
            if (uniformMap->size() <= i) {
                uniformMap->push_back(stageUniformMap[i]);
            }
            if (uniformMap->at(i).glsl_names.size() < stageUniformMap.at(i).glsl_names.size()) {
                uniformMap->at(i).glsl_names = stageUniformMap.at(i).glsl_names;
            }
            thing << std::format("- {}: {}", stageUniformMap[i].slang_name, stageUniformMap[i].glsl_names) << "\n";
        }

        thing << "END THING\n";
        std::cout << thing.str() << std::endl;
    }
    for (int i = 0; i < uniformMap->size(); i++) {
        std::cout << std::format("- {}: {}", uniformMap->at(i).slang_name, uniformMap->at(i).glsl_names) << std::endl;
    }
        
    return {
        .glsl = final_code.str(),
        .stages = stages,
        .uniform_map = uniformMap,
    };
}

SlangCompiler::~SlangCompiler() {
    session.setNull();
    std::cout << "Destroying the session..." << std::endl;
}

static void createGlobalSession(Slang::ComPtr<slang::IGlobalSession>& ptr) {
    slang::createGlobalSession(ptr.writeRef());
}

void SlangCompiler::ensureGlobalSession() {
    std::call_once(globalSessionCreated, createGlobalSession, globalSession);
    if (globalSession.get() == nullptr) {
        throw love::Exception("Attempt to create global session after it was destroyed");
    }
}

void SlangCompiler::destroyGlobalSession() {
    globalSession.setNull();
}

} // loveslang
