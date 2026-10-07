#include "common/runtime.h"
#include "internal/SlangCompiler.hpp"
#include "internal/wrap_SlangCompiler.hpp"
#include <lua.h>

namespace loveslang::luawrap {

int luax_checkcompileroptions(lua_State* L, int index, SlangCompilerOptions* options) {
    lua_getfield(L, index, "paths");
    if (!lua_isnoneornil(L, -1) && lua_type(L, -1) == LUA_TTABLE) {
        if (!lua_istable(L, -1))
            luaL_argerror(L, index, "expected 'paths' field to be a table");

			lua_pushnil(L);

			while (lua_next(L, -2))
			{
                int index = 0;
                index = luaL_checkinteger(L, -2);
				std::string path = love::luax_tostring(L, -1);

                if (!options->search_paths) {
                    options->search_paths = std::make_shared<std::vector<std::string>>();
                }
				options->search_paths->push_back(std::move(path));

				lua_pop(L, 1);
			}
    }
    lua_pop(L, 1);
    return 0;
}

int w_newCompiler(lua_State* L) {
    SlangCompilerOptions options{};
    if (lua_type(L, 1) == LUA_TTABLE) {
        luax_checkcompileroptions(L, 1, &options);
    }
    try {
        loveslang::SlangCompiler* session = new loveslang::SlangCompiler(&options);
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

