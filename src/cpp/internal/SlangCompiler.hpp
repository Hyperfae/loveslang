#pragma once

#include "common/Object.h"
#include "graphics/ShaderStage.h"
#include "slang.h"
#include "slang-com-ptr.h"
#include <memory>
#include <mutex>
#include <string>

namespace loveslang {


struct UniformInfo {
    std::string slang_name;
    std::vector<std::string> glsl_names;
};

struct SlangCompilerOutput {
    std::string glsl;
    std::bitset<love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> stages;
    std::shared_ptr<std::vector<UniformInfo>> uniform_map;
};

class SlangCompiler : public love::Object
{
public:
	static love::Type type;
	SlangCompiler();
	virtual ~SlangCompiler();

    SlangCompilerOutput compileToGLSL(std::string_view moduleName);
    SlangCompilerOutput getCompilerOutputFromModule(Slang::ComPtr<slang::IModule> module);

    Slang::ComPtr<slang::ISession> session;

    static void ensureGlobalSession();
    // Destroys the global session used by loveslang. Should only be called when you're 100% sure the program is about to end.
    static void destroyGlobalSession();
protected:
    virtual std::string postprocessStageCode(std::string_view inCode, love::graphics::ShaderStageType stage);
private:
    struct StageInfo {
        std::string glsl;
        Slang::ComPtr<slang::IComponentType> linkedProgram;
        love::graphics::ShaderStageType stage;
    };
    StageInfo getRawStageCode(slang::IModule* slangModule, slang::IEntryPoint* entryPoint);
    std::vector<UniformInfo> createUniformMap(StageInfo* stageinfo);

    static Slang::ComPtr<slang::IGlobalSession> globalSession;
    static std::once_flag globalSessionCreated;
};

}
