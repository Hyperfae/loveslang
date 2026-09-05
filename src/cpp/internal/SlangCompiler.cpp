#include "SlangCompiler.hpp"
#include "DataSlangBlob.hpp"
#include "common/Exception.h"
#include "LOVESlangFilesystem.hpp"
#include "graphics/Shader.h"
#include "graphics/ShaderStage.h"
#include <array>
#include <iostream>
#include <mutex>
#include <regex>
#include <slang-com-ptr.h>
#include <slang.h>
#include <sstream>
#include <string>
#include <string_view>
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

static std::string postprocessStageCode(std::string_view inCode, love::graphics::ShaderStageType stage) {
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

    return code;
}

static void assertSlangOK(SlangResult result, slang::IBlob* diagnosticsBlob) {
    if (result == SLANG_OK) return;
    if (!diagnosticsBlob)
        throw love::Exception("Slang failure (%#08x): Unknown", result);
    throw love::Exception("Slang failure (%#08x): %s", result, std::string_view((char*)diagnosticsBlob->getBufferPointer(), diagnosticsBlob->getBufferSize()));
}

std::string SlangCompiler::getRawStageCode(slang::IModule* slangModule, slang::IEntryPoint* entryPoint) {
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
    // Slang::ComPtr<slang::IBlob> outDiag;
    // auto result = entrypoint->getTargetCode(0, outCode.writeRef(), outDiag.writeRef());
    // if (result != SLANG_OK) {
    //     throw love::Exception("Failed to retrieve target code (%#08x): %s", result, "Unknown");
    // }
    
    return std::string((char*)outCode->getBufferPointer(), outCode->getBufferSize());
    // return "";
}

SlangCompilerOutput SlangCompiler::getCompilerOutputFromModule(Slang::ComPtr<slang::IModule> module) {
    auto entrypoinsByStage = getEntryPoints(module);
    std::stringstream final_code{};
    static std::array<const char *, love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> stageNames {
        "VERTEX",
        "PIXEL",
        "COMPUTE"
    };
    for (int stage_id = 0; stage_id < love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM; stage_id++) {
        if (!entrypoinsByStage[stage_id])
            continue;
        auto stage = (love::graphics::ShaderStageType)stage_id;
        final_code << "#ifdef " << stageNames[stage_id] << "\n";
        final_code << postprocessStageCode(getRawStageCode(module, entrypoinsByStage[stage_id].get()), stage);
        final_code << "\n#endif\n";
    }
    return {
        .glsl = final_code.str(),
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
