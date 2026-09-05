#include <lua.hpp>
#include "wrap_SlangCompiler.hpp"
#include "SlangCompiler.hpp"
#include "LOVESlangFilesystem.hpp"
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
	auto a = new LOVESlangFilesystem();
	auto compiler = love::luax_checktype<SlangCompiler>(L, 1);
	std::string path = love::luax_checkstring(L, 2);
	Slang::ComPtr<ISlangBlob> blobptr;
	auto status = a->loadFile(path.c_str(), blobptr.writeRef());
	if (status == SLANG_OK && blobptr.get() != nullptr) {
		auto ptr = blobptr->getBufferPointer();
		std::cout << "hi" << std::endl;
		std::cout << std::string_view((const char*)blobptr->getBufferPointer(), blobptr->getBufferSize()) << std::endl;
		std::cout << "bye" << std::endl;
	} else {
		std::cout << "slangfail:" << status << std::endl;
	}
	return 0;
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
