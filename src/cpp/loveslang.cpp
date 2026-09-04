#include "./internal/SlangCompiler.hpp"

int main() {
    auto stronkmeeks = love::StrongRef(new loveslang::SlangCompiler(), love::Acquire::NORETAIN);
    loveslang::SlangCompiler::destroyGlobalSession();
    // auto stronkmeeks2 = love::StrongRef(new loveslang::SlangSession(), love::Acquire::NORETAIN);
    return 0;
}
