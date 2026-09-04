#include "common/runtime.h"
#include "internal/SlangCompiler.hpp"
#include "internal/wrap_SlangCompiler.hpp"
#include <lua.h>

namespace loveslang::luawrap {

int w_newCompiler(lua_State* L) {
    try {
        loveslang::SlangCompiler* session = new loveslang::SlangCompiler();
        love::luax_pushtype(L, session);
        session->release();
        return 1;
        
    }
    catch (const std::exception& e) {
        lua_pushstring(L, e.what());
        return lua_error(L);
    }
}

static const luaL_Reg loveslang_funcs[] = {
    {"newCompiler", w_newCompiler},
    {0, 0}
};

LOVE_EXPORT extern "C" int luaopen_loveslang(lua_State* L) {
    luaopen_slangsession(L);
    lua_newtable(L);
    love::luax_setfuncs(L, loveslang_funcs);
    return 1;
}

} // loveslang::luawrap

