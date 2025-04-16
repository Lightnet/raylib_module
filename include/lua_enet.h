#ifndef LUA_ENET_H
#define LUA_ENET_H

//#define ENET_IMPLEMENTATION //need this to remove warning.  unresolved external symbol

#include <lua.h>
#define ENET_IMPLEMENTATION
#include <enet.h>
#undef ENET_IMPLEMENTATION

int luaopen_enet(lua_State *L);

#endif