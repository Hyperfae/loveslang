#pragma once
#include <luajit.h>
#include "common/config.h"
// #include "common/runtime.h"

extern "C" LOVE_EXPORT int luaopen_slangsession(lua_State *L);
