#ifndef LUAJIT_UTILS_H
#define LUAJIT_UTILS_H

#include <lauxlib.h>
#include <raylib.h>

// Helper to unpack Color {r, g, b, a}
static Color unpack_color(lua_State *L, int idx) {
    Color c = {0, 0, 0, 255};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); c.r = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); c.g = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 3); c.b = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 4); c.a = lua_isnil(L, -1) ? 255 : (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
    }
    return c;
}

// Helper to push Color {r, g, b, a}
static void push_color(lua_State *L, Color c) {
    lua_newtable(L);
    lua_pushinteger(L, c.r); lua_rawseti(L, -2, 1);
    lua_pushinteger(L, c.g); lua_rawseti(L, -2, 2);
    lua_pushinteger(L, c.b); lua_rawseti(L, -2, 3);
    lua_pushinteger(L, c.a); lua_rawseti(L, -2, 4);
}

// Helper to unpack Rectangle {x, y, width, height}
static Rectangle unpack_rectangle(lua_State *L, int idx) {
    Rectangle r = {0, 0, 0, 0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); r.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); r.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 3); r.width = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 4); r.height = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return r;
}

// Helper to push Rectangle {x, y, width, height}
static void push_rectangle(lua_State *L, Rectangle r) {
    lua_newtable(L);
    lua_pushnumber(L, r.x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, r.y); lua_rawseti(L, -2, 2);
    lua_pushnumber(L, r.width); lua_rawseti(L, -2, 3);
    lua_pushnumber(L, r.height); lua_rawseti(L, -2, 4);
}

// Helper to unpack Vector2 {x, y}
static Vector2 unpack_vector2(lua_State *L, int idx) {
    Vector2 v = {0, 0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return v;
}

// Helper to push Vector2 {x, y}
static void push_vector2(lua_State *L, Vector2 v) {
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, v.y); lua_rawseti(L, -2, 2);
}

#endif