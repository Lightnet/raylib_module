#include "lua_raymath.h"
#include <raymath.h>
#include <lauxlib.h>

// Helper to unpack Vector2 from Lua table {x, y}
static Vector2 unpack_vector2(lua_State *L, int idx) {
    Vector2 v = {0, 0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return v;
}

// Helper to push Vector2 to Lua as a table {x, y}
static void push_vector2(lua_State *L, Vector2 v) {
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, v.y); lua_rawseti(L, -2, 2);
}

// Helper to unpack Vector3 from Lua table {x, y, z}
static Vector3 unpack_vector3(lua_State *L, int idx) {
    Vector3 v = {0, 0, 0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 3); v.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return v;
}

// Helper to push Vector3 to Lua as a table {x, y, z}
static void push_vector3(lua_State *L, Vector3 v) {
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, v.y); lua_rawseti(L, -2, 2);
    lua_pushnumber(L, v.z); lua_rawseti(L, -2, 3);
}

// Helper to unpack Vector4 from Lua table {x, y, z, w}
static Vector4 unpack_vector4(lua_State *L, int idx) {
    Vector4 v = {0, 0, 0, 0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 3); v.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 4); v.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return v;
}

// Helper to push Vector4 to Lua as a table {x, y, z, w}
static void push_vector4(lua_State *L, Vector4 v) {
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, v.y); lua_rawseti(L, -2, 2);
    lua_pushnumber(L, v.z); lua_rawseti(L, -2, 3);
    lua_pushnumber(L, v.w); lua_rawseti(L, -2, 4);
}

// Helper to unpack Matrix from Lua table {m0, m4, m8, m12, m1, m5, m9, m13, m2, m6, m10, m14, m3, m7, m11, m15}
static Matrix unpack_matrix(lua_State *L, int idx) {
    Matrix m = {0};
    if (lua_istable(L, idx)) {
        lua_rawgeti(L, idx, 1);  m.m0  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 2);  m.m4  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 3);  m.m8  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 4);  m.m12 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 5);  m.m1  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 6);  m.m5  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 7);  m.m9  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 8);  m.m13 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 9);  m.m2  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 10); m.m6  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 11); m.m10 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 12); m.m14 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 13); m.m3  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 14); m.m7  = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 15); m.m11 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, idx, 16); m.m15 = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return m;
}

// Helper to push Matrix to Lua as a table
static void push_matrix(lua_State *L, Matrix m) {
    lua_newtable(L);
    lua_pushnumber(L, m.m0);  lua_rawseti(L, -2, 1);
    lua_pushnumber(L, m.m4);  lua_rawseti(L, -2, 2);
    lua_pushnumber(L, m.m8);  lua_rawseti(L, -2, 3);
    lua_pushnumber(L, m.m12); lua_rawseti(L, -2, 4);
    lua_pushnumber(L, m.m1);  lua_rawseti(L, -2, 5);
    lua_pushnumber(L, m.m5);  lua_rawseti(L, -2, 6);
    lua_pushnumber(L, m.m9);  lua_rawseti(L, -2, 7);
    lua_pushnumber(L, m.m13); lua_rawseti(L, -2, 8);
    lua_pushnumber(L, m.m2);  lua_rawseti(L, -2, 9);
    lua_pushnumber(L, m.m6);  lua_rawseti(L, -2, 10);
    lua_pushnumber(L, m.m10); lua_rawseti(L, -2, 11);
    lua_pushnumber(L, m.m14); lua_rawseti(L, -2, 12);
    lua_pushnumber(L, m.m3);  lua_rawseti(L, -2, 13);
    lua_pushnumber(L, m.m7);  lua_rawseti(L, -2, 14);
    lua_pushnumber(L, m.m11); lua_rawseti(L, -2, 15);
    lua_pushnumber(L, m.m15); lua_rawseti(L, -2, 16);
}

// Vector2 functions
static int l_Vector2Add(lua_State *L) {
    Vector2 v1 = unpack_vector2(L, 1);
    Vector2 v2 = unpack_vector2(L, 2);
    Vector2 result = Vector2Add(v1, v2);
    push_vector2(L, result);
    return 1;
}

static int l_Vector2Subtract(lua_State *L) {
    Vector2 v1 = unpack_vector2(L, 1);
    Vector2 v2 = unpack_vector2(L, 2);
    Vector2 result = Vector2Subtract(v1, v2);
    push_vector2(L, result);
    return 1;
}

static int l_Vector2Scale(lua_State *L) {
    Vector2 v = unpack_vector2(L, 1);
    float scale = (float)luaL_checknumber(L, 2);
    Vector2 result = Vector2Scale(v, scale);
    push_vector2(L, result);
    return 1;
}

static int l_Vector2Length(lua_State *L) {
    Vector2 v = unpack_vector2(L, 1);
    float length = Vector2Length(v);
    lua_pushnumber(L, length);
    return 1;
}

static int l_Vector2Normalize(lua_State *L) {
    Vector2 v = unpack_vector2(L, 1);
    Vector2 result = Vector2Normalize(v);
    push_vector2(L, result);
    return 1;
}

// Vector3 functions
static int l_Vector3Add(lua_State *L) {
    Vector3 v1 = unpack_vector3(L, 1);
    Vector3 v2 = unpack_vector3(L, 2);
    Vector3 result = Vector3Add(v1, v2);
    push_vector3(L, result);
    return 1;
}

static int l_Vector3Subtract(lua_State *L) {
    Vector3 v1 = unpack_vector3(L, 1);
    Vector3 v2 = unpack_vector3(L, 2);
    Vector3 result = Vector3Subtract(v1, v2);
    push_vector3(L, result);
    return 1;
}

static int l_Vector3Scale(lua_State *L) {
    Vector3 v = unpack_vector3(L, 1);
    float scale = (float)luaL_checknumber(L, 2);
    Vector3 result = Vector3Scale(v, scale);
    push_vector3(L, result);
    return 1;
}

// Matrix functions
static int l_MatrixIdentity(lua_State *L) {
    Matrix result = MatrixIdentity();
    push_matrix(L, result);
    return 1;
}

static int l_MatrixMultiply(lua_State *L) {
    Matrix m1 = unpack_matrix(L, 1);
    Matrix m2 = unpack_matrix(L, 2);
    Matrix result = MatrixMultiply(m1, m2);
    push_matrix(L, result);
    return 1;
}

static const luaL_Reg raymath_funcs[] = {
    {"Vector2Add", l_Vector2Add},
    {"Vector2Subtract", l_Vector2Subtract},
    {"Vector2Scale", l_Vector2Scale},
    {"Vector2Length", l_Vector2Length},
    {"Vector2Normalize", l_Vector2Normalize},
    {"Vector3Add", l_Vector3Add},
    {"Vector3Subtract", l_Vector3Subtract},
    {"Vector3Scale", l_Vector3Scale},
    {"MatrixIdentity", l_MatrixIdentity},
    {"MatrixMultiply", l_MatrixMultiply},
    {NULL, NULL}
};

int luaopen_raymath(lua_State *L) {
    lua_newtable(L);
    luaL_setfuncs(L, raymath_funcs, 0);
    return 1;
}