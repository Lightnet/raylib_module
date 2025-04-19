// #define _CRT_SECURE_NO_WARNINGS

#include <raylib.h>
// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"
// #undef RAYGUI_IMPLEMENTATION
#include "lua_utils.h"

#define MAX_TEXT_LENGTH 256

// Basic Controls
static int l_GuiLabel(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    GuiLabel(bounds, text);
    return 0;
}

static int l_GuiButton(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    int clicked = GuiButton(bounds, text);
    lua_pushboolean(L, clicked);
    return 1;
}

static int l_GuiCheckBox(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    bool checked = lua_toboolean(L, 3);
    int result = GuiCheckBox(bounds, text, &checked);
    lua_pushboolean(L, checked);
    return 1;
}

static int l_GuiToggle(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    bool active = lua_toboolean(L, 3);
    int result = GuiToggle(bounds, text, &active);
    lua_pushboolean(L, active);
    return 1;
}

static int l_GuiSlider(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *textLeft = luaL_optstring(L, 2, NULL);
    const char *textRight = luaL_optstring(L, 3, NULL);
    float value = (float)luaL_checknumber(L, 4);
    float minValue = (float)luaL_checknumber(L, 5);
    float maxValue = (float)luaL_checknumber(L, 6);
    GuiSlider(bounds, textLeft, textRight, &value, minValue, maxValue);
    lua_pushnumber(L, value);
    return 1;
}

static int l_GuiProgressBar(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *textLeft = luaL_optstring(L, 2, NULL);
    const char *textRight = luaL_optstring(L, 3, NULL);
    float value = (float)luaL_checknumber(L, 4);
    float minValue = (float)luaL_checknumber(L, 5);
    float maxValue = (float)luaL_checknumber(L, 6);
    GuiProgressBar(bounds, textLeft, textRight, &value, minValue, maxValue);
    lua_pushnumber(L, value);
    return 1;
}

static int l_GuiSpinner(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_optstring(L, 2, NULL);
    int value = luaL_checkinteger(L, 3);
    int minValue = luaL_checkinteger(L, 4);
    int maxValue = luaL_checkinteger(L, 5);
    int changed = GuiSpinner(bounds, text, &value, minValue, maxValue, false);
    lua_pushinteger(L, value);
    return 1;
}

static int l_GuiTextBox(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    char text[MAX_TEXT_LENGTH] = {0};
    strncpy(text, luaL_checkstring(L, 2), MAX_TEXT_LENGTH - 1);
    bool editMode = lua_toboolean(L, 3);
    int result = GuiTextBox(bounds, text, MAX_TEXT_LENGTH, &editMode);
    lua_pushstring(L, text);
    lua_pushboolean(L, editMode);
    return 2;
}

// Container Controls
static int l_GuiWindowBox(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *title = luaL_checkstring(L, 2);
    int result = GuiWindowBox(bounds, title);
    lua_pushboolean(L, result);
    return 1;
}

static int l_GuiGroupBox(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    GuiGroupBox(bounds, text);
    return 0;
}

static int l_GuiScrollPanel(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_optstring(L, 2, NULL);
    Rectangle content = unpack_rectangle(L, 3);
    Vector2 scroll = unpack_vector2(L, 4);
    Rectangle view = {0};
    int result = GuiScrollPanel(bounds, text, content, &scroll, &view);
    lua_pushinteger(L, result);
    push_vector2(L, scroll);
    push_rectangle(L, view);
    return 3;
}

// Selection Controls
static int l_GuiDropdownBox(lua_State *L) {
  Rectangle bounds = unpack_rectangle(L, 1);
  const char *text = luaL_checkstring(L, 2);
  int active = luaL_checkinteger(L, 3);
  bool editMode = lua_toboolean(L, 4);
  int pre_active = active;  // Log before
  int result = GuiDropdownBox(bounds, text, &active, editMode);
  //TraceLog(LOG_INFO, "Dropdown: pre_active=%d, post_active=%d, result=%d", pre_active, active, result);
  lua_pushinteger(L, active);
  lua_pushboolean(L, result);
  return 2;
}

static int l_GuiListView(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_checkstring(L, 2);
    int scrollIndex = luaL_checkinteger(L, 3);
    int active = luaL_checkinteger(L, 4);
    int result = GuiListView(bounds, text, &scrollIndex, &active);
    lua_pushinteger(L, scrollIndex);
    lua_pushinteger(L, active);
    return 2;
}

static int l_GuiColorPicker(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *text = luaL_optstring(L, 2, NULL);
    Color color = unpack_color(L, 3);
    GuiColorPicker(bounds, text, &color);
    push_color(L, color);
    return 1;
}

// Additional Controls
static int l_GuiMessageBox(lua_State *L) {
    Rectangle bounds = unpack_rectangle(L, 1);
    const char *title = luaL_checkstring(L, 2);
    const char *message = luaL_checkstring(L, 3);
    const char *buttons = luaL_checkstring(L, 4);
    int result = GuiMessageBox(bounds, title, message, buttons);
    lua_pushinteger(L, result);
    return 1;
}

static const luaL_Reg raygui_funcs[] = {
    {"GuiLabel", l_GuiLabel},
    {"GuiButton", l_GuiButton},
    {"GuiCheckBox", l_GuiCheckBox},
    {"GuiToggle", l_GuiToggle},
    {"GuiSlider", l_GuiSlider},
    {"GuiProgressBar", l_GuiProgressBar},
    {"GuiSpinner", l_GuiSpinner},
    {"GuiTextBox", l_GuiTextBox},
    {"GuiWindowBox", l_GuiWindowBox},
    {"GuiGroupBox", l_GuiGroupBox},
    {"GuiScrollPanel", l_GuiScrollPanel},
    {"GuiDropdownBox", l_GuiDropdownBox},
    {"GuiListView", l_GuiListView},
    {"GuiColorPicker", l_GuiColorPicker},
    {"GuiMessageBox", l_GuiMessageBox},
    {NULL, NULL}
};

int luaopen_raygui(lua_State *L) {
    lua_newtable(L);
    luaL_setfuncs(L, raygui_funcs, 0);
    return 1;
}