#ifndef ENET_LUAJIT_H
#define ENET_LUAJIT_H

//#define ENET_IMPLEMENTATION //need this to remove warning.  unresolved external symbol

#include <lua.h>
// #define ENET_IMPLEMENTATION
#include <enet.h>

int luaopen_enet(lua_State *L);

#endif