#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#define ENET_IMPLEMENTATION
#include <enet.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "mimalloc.h"

#include "raylib_lua.h"
#include "enet_lua.h"
#include "raymath_lua.h"
#include "raygui_lua.h"
#include "flecs_lua.h"

// Custom allocator using mimalloc
static void *mimalloc_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud; // Unused user data
  if (nsize == 0) {
      mi_free(ptr);
      return NULL;
  } else if (ptr == NULL) {
      return mi_malloc(nsize);
  } else {
      return mi_realloc(ptr, nsize);
  }
}

// Custom error handler
static int lua_error_handler(lua_State *L) {
    const char *msg = NULL;

    int top = lua_gettop(L);
    printf("Error handler called, stack top=%d\n", top);
    if (top < 1) {
        msg = "(no error object on stack)";
        fprintf(stderr, "Lua Error: %s\n", msg);
    } else {
        msg = lua_tostring(L, -1);
        if (!msg) {
            int type = lua_type(L, -1);
            switch (type) {
                case LUA_TNIL: msg = "(nil)"; break;
                case LUA_TNUMBER: msg = "(number)"; break;
                case LUA_TSTRING: msg = "(string conversion failed)"; break;
                case LUA_TBOOLEAN: msg = lua_toboolean(L, -1) ? "(true)" : "(false)"; break;
                case LUA_TTABLE: msg = "(table)"; break;
                case LUA_TFUNCTION: msg = "(function)"; break;
                case LUA_TUSERDATA: msg = "(userdata)"; break;
                case LUA_TLIGHTUSERDATA: msg = "(lightuserdata)"; break;
                case LUA_TTHREAD: msg = "(thread)"; break;
                default: msg = "(unknown error object)"; break;
            }
        }
        fprintf(stderr, "Lua Error: %s\n", msg);
    }

    lua_Debug ar;
    int level = 0;
    fprintf(stderr, "Stack Trace:\n");
    while (lua_getstack(L, level, &ar)) {
        if (lua_getinfo(L, "nSl", &ar)) {
            fprintf(stderr, "  %d: %s:%d - %s\n",
                    level,
                    ar.short_src ? ar.short_src : "(unknown source)",
                    ar.currentline >= 0 ? ar.currentline : 0,
                    ar.name ? ar.name : "(anonymous)");
        }
        level++;
    }
    if (level == 0) {
        fprintf(stderr, "  (no stack trace available)\n");
    }

    lua_pushstring(L, msg ? msg : "(unknown error)");
    return 1;
}

int main(int argc, char *argv[]) {
    // printf("LUA_TNONE %d\n", LUA_TNONE);
    // printf("LUA_TNIL %d\n", LUA_TNIL);
    // printf("LUA_TBOOLEAN %d\n", LUA_TBOOLEAN);
    // printf("LUA_TNUMBER %d\n", LUA_TNUMBER);
    // printf("LUA_TSTRING %d\n", LUA_TSTRING);
    // printf("LUA_TTABLE %d\n", LUA_TTABLE);
    // printf("LUA_TFUNCTION %d\n", LUA_TFUNCTION);
    // printf("LUA_TUSERDATA %d\n", LUA_TUSERDATA);
    // printf("LUA_TTHREAD %d\n", LUA_TTHREAD);
    // printf("LUA_TTABLE %d\n", LUA_TTABLE);

    const char *script_path = "demo.lua";

    if (argc > 1) {
        script_path = argv[1];
        const char *ext = strrchr(script_path, '.');
        if (!ext || strcmp(ext, ".lua") != 0) {
            fprintf(stderr, "Error: Script '%s' must have a .lua extension\n", script_path);
            return 1;
        }
    }

    // Initialize Lua with mimalloc allocator
    //lua_State *L = luaL_newstate();
    lua_State *L = lua_newstate(mimalloc_lua_alloc, NULL);
    if (!L) {
        fprintf(stderr, "Failed to create LuaJIT state\n");
        return 1;
    }

    luaL_openlibs(L);

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, luaopen_raylib);
    lua_setfield(L, -2, "rl");
    lua_pushcfunction(L, luaopen_enet);
    lua_setfield(L, -2, "enet");
    lua_pushcfunction(L, luaopen_raymath);
    lua_setfield(L, -2, "rm");
    lua_pushcfunction(L, luaopen_raygui);
    lua_setfield(L, -2, "rg");
    lua_pushcfunction(L, luaopen_flecs);
    lua_setfield(L, -2, "flecs_lua");
    lua_pop(L, 2);

    //printf("Stack before pcall: top=%d\n", lua_gettop(L));
    lua_pushcfunction(L, lua_error_handler);
    //printf("Running script, stack top=%d\n", lua_gettop(L));

    printf("Loading script '%s'...\n", script_path);
    if (luaL_loadfile(L, script_path) != LUA_OK) {
        fprintf(stderr, "Error loading script '%s': %s\n", script_path, lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    int status = lua_pcall(L, 0, 0, -2);
    //printf("lua_pcall returned %d, stack top=%d\n", status, lua_gettop(L));
    if (status != LUA_OK) {
        lua_close(L);
        fprintf(stderr, "Program exited with error code %d\n", status);
        return 1;
    }

    lua_close(L);
    printf("Program ran successfully!\n");
    return 0;
}