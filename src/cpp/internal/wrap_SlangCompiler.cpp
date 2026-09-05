#include <lua.hpp>
#include "wrap_SlangCompiler.hpp"
#include "SlangCompiler.hpp"
#include "LOVESlangFilesystem.hpp"
#include "common/Exception.h"
#include "modules/filesystem/wrap_Filesystem.h"
#include "common/runtime.h"
#include <iostream>
#include <lauxlib.h>
#include <lua.h>
#include <ostream>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string_view>

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
	return 1;
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
