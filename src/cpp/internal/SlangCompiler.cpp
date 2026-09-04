#include "SlangCompiler.hpp"
#include "common/Exception.h"
#include "graphics/Shader.h"
#include "graphics/ShaderStage.h"
#include <array>
#include <iostream>
#include <mutex>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <string_view>
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
    sessionDesc.targetCount = 1;
    std::array<slang::PreprocessorMacroDesc, 1> preprocessorMacroDesc =
    {
        { "LOVESLANG", "LOVESLANG" },
    };
    sessionDesc.preprocessorMacros = preprocessorMacroDesc.data();
    sessionDesc.preprocessorMacroCount = preprocessorMacroDesc.size();
    globalSession->createSession(sessionDesc,
                                 session.writeRef());
}

std::string SlangCompiler::compileToGLSL(std::string_view path, std::string_view source) {
    return "hello void main() yay";
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
