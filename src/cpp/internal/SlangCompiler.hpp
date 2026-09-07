#pragma once

#include "common/Data.h"
#include "common/Object.h"
#include "graphics/ShaderStage.h"
#include "slang.h"
#include "slang-com-ptr.h"
#include <mutex>
#include <string>

namespace loveslang {

struct SlangCompilerOutput {
    std::string glsl;
    std::bitset<love::graphics::ShaderStageType::SHADERSTAGE_MAX_ENUM> stages;
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
    std::string getRawStageCode(slang::IModule* slangModule, slang::IEntryPoint* entryPoint);

    static Slang::ComPtr<slang::IGlobalSession> globalSession;
    static std::once_flag globalSessionCreated;
};

}
