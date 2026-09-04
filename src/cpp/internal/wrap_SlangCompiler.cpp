#include "wrap_SlangCompiler.hpp"
#include "SlangCompiler.hpp"
#include "LOVESlangFilesystem.hpp"
#include "common/runtime.h"
#include <cstddef>
#include <iostream>
#include <lauxlib.h>
#include <lua.h>
#include <ostream>
#include <slang-com-ptr.h>
#include <slang.h>

namespace loveslang
{

static int w_compileGLSL(lua_State* L)
{
	auto a = new LOVESlangFilesystem();
	Slang::ComPtr<ISlangBlob> blobptr;
	auto status = a->loadFile("main.lua", blobptr.writeRef());
	if (status == SLANG_OK && blobptr.get() != nullptr) {
		auto ptr = blobptr->getBufferPointer();
		std::cout << "hi" << std::endl;
		std::cout << (char *)(blobptr->getBufferPointer()) << std::endl;
		std::cout << "bye" << std::endl;
	} else {
		std::cout << "slangfail:" << status << std::endl;
	}
	return 0;
}

static const luaL_Reg w_SlangSession_functions[] =
{
	{"compileGLSL", w_compileGLSL},
	{ 0, 0 }
};
extern "C" int luaopen_slangsession(lua_State *L)
{
	return love::luax_register_type(L, &SlangCompiler::type, w_SlangSession_functions, nullptr);
}

} //loveslang
