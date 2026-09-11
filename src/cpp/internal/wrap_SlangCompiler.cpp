#include <lua.hpp>
#include "wrap_SlangCompiler.hpp"
#include "SlangCompiler.hpp"
#include "common/Exception.h"
#include "common/runtime.h"
#include <lauxlib.h>
#include <lua.h>
#include <slang-com-ptr.h>
#include <slang.h>

namespace loveslang
{
static int w_compileToGLSL(lua_State* L)
{
	auto compiler = love::luax_checktype<SlangCompiler>(L, 1);
	std::string moduleName = love::luax_checkstring(L, 2);
	Slang::ComPtr<ISlangBlob> blobptr;
	
	SlangCompilerOutput compiled;
	try {
		compiled = compiler->compileToGLSL(moduleName);
	} catch (love::Exception e) {
		lua_pushstring(L, e.what());
		lua_error(L);
	}
	love::luax_pushstring(L, compiled.glsl);
	// TODO: Probably redesign this reflection format
	lua_createtable(L, 0, compiled.uniform_map->size());
	for (int i = 0; i < compiled.uniform_map->size(); i++) {
		auto info = compiled.uniform_map->at(i);
		lua_createtable(L, 0, 2);
		love::luax_pushstring(L, "name");
		love::luax_pushstring(L, info.glsl_names[info.glsl_names.size() - 1]);
		lua_settable(L, -3);
		if (info.glsl_names.size() > 1) {
			love::luax_pushstring(L, "counter_name");
			love::luax_pushstring(L, info.glsl_names[0]);
			lua_settable(L, -3);
		}
		lua_setfield(L, -2, info.slang_name.data());
	}
	return 2;
}

static const luaL_Reg w_SlangSession_functions[] =
{
	{"compileToGLSL", w_compileToGLSL},
	{ 0, 0 }
};
extern "C" int luaopen_slangsession(lua_State *L)
{
	return love::luax_register_type(L, &SlangCompiler::type, w_SlangSession_functions, nullptr);
}

} //loveslang
