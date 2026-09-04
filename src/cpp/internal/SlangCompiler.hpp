#pragma once

#include "common/Object.h"
#include "modules/graphics/Shader.h"
#include "common/runtime.h"
#include "slang.h"
#include "slang-com-ptr.h"
#include <mutex>

namespace loveslang {

class SlangCompiler : public love::Object
{
public:
	static love::Type type;
	SlangCompiler();
	virtual ~SlangCompiler();

    std::string compileToGLSL(std::string_view path, std::string_view source);

    Slang::ComPtr<slang::ISession> session;

    static void ensureGlobalSession();
    // Destroys the global session used by loveslang. Should only be called when you're 100% sure the program is about to end.
    static void destroyGlobalSession();
private:
    static Slang::ComPtr<slang::IGlobalSession> globalSession;
    static std::once_flag globalSessionCreated;
};

}
