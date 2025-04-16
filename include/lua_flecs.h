#ifndef LUA_FLECS_H
#define LUA_FLECS_H

// Metatable name for ecs_world_t
//#define ECS_WORLD_MT "ecs_world_t"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "flecs.h"

// Register the Flecs Lua module
int luaopen_flecs(lua_State *L);

#endif // FLECS_LUA_H