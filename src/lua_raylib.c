#include "lua_raylib.h"
#include "lua_utils.h"
#include <raylib.h>

// Core
static int l_InitWindow(lua_State *L) {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);
    InitWindow(width, height, title);
    return 0;
}

static int l_CloseWindow(lua_State *L) {
  CloseWindow();
  return 0;
}

static int l_WindowShouldClose(lua_State *L) {
    lua_pushboolean(L, WindowShouldClose());
    return 1;
}

static int l_IsWindowReady(lua_State *L) {
  lua_pushboolean(L, IsWindowReady());
  return 1;
}

static int l_ToggleFullscreen(lua_State *L) {
  ToggleFullscreen();
  return 0;
}

// Window Management
static int l_IsWindowFullscreen(lua_State *L) {
  lua_pushboolean(L, IsWindowFullscreen());
  return 1;
}

static int l_GetScreenWidth(lua_State *L) {
  lua_pushinteger(L, GetScreenWidth());
  return 1;
}

static int l_GetScreenHeight(lua_State *L) {
  lua_pushinteger(L, GetScreenHeight());
  return 1;
}

static int l_SetWindowPosition(lua_State *L) {
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  SetWindowPosition(x, y);
  return 0;
}

static int l_SetWindowSize(lua_State *L) {
  int width = luaL_checkinteger(L, 1);
  int height = luaL_checkinteger(L, 2);
  SetWindowSize(width, height);
  return 0;
}

static int l_MaximizeWindow(lua_State *L) {
  MaximizeWindow();
  return 0;
}

static int l_MinimizeWindow(lua_State *L) {
  MinimizeWindow();
  return 0;
}

// Timing
static int l_GetTime(lua_State *L) {
  lua_pushnumber(L, GetTime());
  return 1;
}

// Miscellaneous
static int l_SetRandomSeed(lua_State *L) {
  unsigned int seed = (unsigned int)luaL_checkinteger(L, 1);
  SetRandomSeed(seed);
  return 0;
}

static int l_GetRandomValue(lua_State *L) {
  int min = luaL_checkinteger(L, 1);
  int max = luaL_checkinteger(L, 2);
  lua_pushinteger(L, GetRandomValue(min, max));
  return 1;
}


static int l_SetWindowTitle(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  SetWindowTitle(title);
  return 0;
}

// Keyboard Functions:
static int l_IsKeyReleased(lua_State *L) {
  int key = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsKeyReleased(key));
  return 1;
}

static int l_IsKeyUp(lua_State *L) {
  int key = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsKeyUp(key));
  return 1;
}

static int l_GetCharPressed(lua_State *L) {
  int charPressed = GetCharPressed();
  if (charPressed == 0) {
      lua_pushnil(L);  // Return nil if no char is pressed
  } else {
      lua_pushinteger(L, charPressed);
  }
  return 1;
}

static int l_SetExitKey(lua_State *L) {
  int key = luaL_checkinteger(L, 1);
  SetExitKey(key);
  return 0;
}

// Mouse Functions:
static int l_IsMouseButtonDown(lua_State *L) {
  int button = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsMouseButtonDown(button));
  return 1;
}

static int l_IsMouseButtonReleased(lua_State *L) {
  int button = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsMouseButtonReleased(button));
  return 1;
}

static int l_IsMouseButtonUp(lua_State *L) {
  int button = luaL_checkinteger(L, 1);
  lua_pushboolean(L, IsMouseButtonUp(button));
  return 1;
}

static int l_GetMouseX(lua_State *L) {
  lua_pushinteger(L, GetMouseX());
  return 1;
}

static int l_GetMouseY(lua_State *L) {
  lua_pushinteger(L, 1);
  lua_pushinteger(L, GetMouseY());
  return 1;
}

static int l_GetMouseWheelMove(lua_State *L) {
  lua_pushnumber(L, GetMouseWheelMove());
  return 1;
}

static int l_SetMousePosition(lua_State *L) {
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  SetMousePosition(x, y);
  return 0;
}

static int l_SetMouseCursor(lua_State *L) {
  int cursor = luaL_checkinteger(L, 1);
  SetMouseCursor(cursor);
  return 0;
}



static int l_SetTargetFPS(lua_State *L) {
    int fps = luaL_checkinteger(L, 1);
    SetTargetFPS(fps);
    return 0;
}

static int l_GetFPS(lua_State *L) {
    lua_pushinteger(L, GetFPS());
    return 1;
}

static int l_GetFrameTime(lua_State *L) {
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

static int l_BeginDrawing(lua_State *L) {
    BeginDrawing();
    return 0;
}

static int l_EndDrawing(lua_State *L) {
    EndDrawing();
    return 0;
}

static int l_ClearBackground(lua_State *L) {
    Color color = unpack_color(L, 1);
    ClearBackground(color);
    return 0;
}

// Input - Keyboard
static int l_IsKeyPressed(lua_State *L) {
    int key = luaL_checkinteger(L, 1);
    lua_pushboolean(L, IsKeyPressed(key));
    return 1;
}

static int l_IsKeyDown(lua_State *L) {
    int key = luaL_checkinteger(L, 1);
    lua_pushboolean(L, IsKeyDown(key));
    return 1;
}

static int l_GetKeyPressed(lua_State *L) {
    lua_pushinteger(L, GetKeyPressed());
    return 1;
}

// Input - Mouse
static int l_IsMouseButtonPressed(lua_State *L) {
    int button = luaL_checkinteger(L, 1);
    lua_pushboolean(L, IsMouseButtonPressed(button));
    return 1;
}

static int l_GetMousePosition(lua_State *L) {
    Vector2 pos = GetMousePosition();
    push_vector2(L, pos);
    return 1;
}

// Shapes
static int l_DrawPixel(lua_State *L) {
    int posX = luaL_checkinteger(L, 1);
    int posY = luaL_checkinteger(L, 2);
    Color color = unpack_color(L, 3);
    DrawPixel(posX, posY, color);
    return 0;
}

static int l_DrawLine(lua_State *L) {
    int startPosX = luaL_checkinteger(L, 1);
    int startPosY = luaL_checkinteger(L, 2);
    int endPosX = luaL_checkinteger(L, 3);
    int endPosY = luaL_checkinteger(L, 4);
    Color color = unpack_color(L, 5);
    DrawLine(startPosX, startPosY, endPosX, endPosY, color);
    return 0;
}

static int l_DrawCircle(lua_State *L) {
    int centerX = luaL_checkinteger(L, 1);
    int centerY = luaL_checkinteger(L, 2);
    float radius = (float)luaL_checknumber(L, 3);
    Color color = unpack_color(L, 4);
    DrawCircle(centerX, centerY, radius, color);
    return 0;
}

static int l_DrawRectangle(lua_State *L) {
    int posX = luaL_checkinteger(L, 1);
    int posY = luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    Color color = unpack_color(L, 5);
    DrawRectangle(posX, posY, width, height, color);
    return 0;
}

static int l_DrawRectangleRec(lua_State *L) {
    Rectangle rec = unpack_rectangle(L, 1);
    Color color = unpack_color(L, 2);
    DrawRectangleRec(rec, color);
    return 0;
}

// Text
static int l_DrawText(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);
    int posX = luaL_checkinteger(L, 2);
    int posY = luaL_checkinteger(L, 3);
    int fontSize = luaL_checkinteger(L, 4);
    Color color = unpack_color(L, 5);
    DrawText(text, posX, posY, fontSize, color);
    return 0;
}

static int l_MeasureText(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);
    int fontSize = luaL_checkinteger(L, 2);
    lua_pushinteger(L, MeasureText(text, fontSize));
    return 1;
}

static int l_TakeScreenshot(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  TakeScreenshot(filename);
  return 0;
}

// 
// Circle variations
static int l_DrawCircleGradient(lua_State *L) {
  int centerX = luaL_checkinteger(L, 1);
  int centerY = luaL_checkinteger(L, 2);
  float radius = (float)luaL_checknumber(L, 3);
  Color color1 = unpack_color(L, 4);
  Color color2 = unpack_color(L, 5);
  DrawCircleGradient(centerX, centerY, radius, color1, color2);
  return 0;
}

static int l_DrawCircleLines(lua_State *L) {
  int centerX = luaL_checkinteger(L, 1);
  int centerY = luaL_checkinteger(L, 2);
  float radius = (float)luaL_checknumber(L, 3);
  Color color = unpack_color(L, 4);
  DrawCircleLines(centerX, centerY, radius, color);
  return 0;
}

// Rectangle variations
static int l_DrawRectangleLines(lua_State *L) {
  int posX = luaL_checkinteger(L, 1);
  int posY = luaL_checkinteger(L, 2);
  int width = luaL_checkinteger(L, 3);
  int height = luaL_checkinteger(L, 4);
  Color color = unpack_color(L, 5);
  DrawRectangleLines(posX, posY, width, height, color);
  return 0;
}

static int l_DrawRectangleGradientV(lua_State *L) {
  int posX = luaL_checkinteger(L, 1);
  int posY = luaL_checkinteger(L, 2);
  int width = luaL_checkinteger(L, 3);
  int height = luaL_checkinteger(L, 4);
  Color color1 = unpack_color(L, 5);
  Color color2 = unpack_color(L, 6);
  DrawRectangleGradientV(posX, posY, width, height, color1, color2);
  return 0;
}

// Triangle
static int l_DrawTriangle(lua_State *L) {
  Vector2 v1 = unpack_vector2(L, 1);
  Vector2 v2 = unpack_vector2(L, 2);
  Vector2 v3 = unpack_vector2(L, 3);
  Color color = unpack_color(L, 4);
  DrawTriangle(v1, v2, v3, color);
  return 0;
}

static int l_DrawTriangleLines(lua_State *L) {
  Vector2 v1 = unpack_vector2(L, 1);
  Vector2 v2 = unpack_vector2(L, 2);
  Vector2 v3 = unpack_vector2(L, 3);
  Color color = unpack_color(L, 4);
  DrawTriangleLines(v1, v2, v3, color);
  return 0;
}

// Poly
static int l_DrawPoly(lua_State *L) {
  Vector2 center = unpack_vector2(L, 1);
  int sides = luaL_checkinteger(L, 2);
  float radius = (float)luaL_checknumber(L, 3);
  float rotation = (float)luaL_checknumber(L, 4);
  Color color = unpack_color(L, 5);
  DrawPoly(center, sides, radius, rotation, color);
  return 0;
}


static const luaL_Reg raylib_funcs[] = {
    {"InitWindow", l_InitWindow},
    {"WindowShouldClose", l_WindowShouldClose},
    {"CloseWindow", l_CloseWindow},
    {"SetTargetFPS", l_SetTargetFPS},
    {"GetFPS", l_GetFPS},
    {"GetFrameTime", l_GetFrameTime},
    {"BeginDrawing", l_BeginDrawing},
    {"EndDrawing", l_EndDrawing},
    {"ClearBackground", l_ClearBackground},
    {"IsKeyPressed", l_IsKeyPressed},
    {"IsKeyDown", l_IsKeyDown},
    {"GetKeyPressed", l_GetKeyPressed},
    {"IsMouseButtonPressed", l_IsMouseButtonPressed},
    {"GetMousePosition", l_GetMousePosition},
    {"DrawPixel", l_DrawPixel},
    {"DrawLine", l_DrawLine},
    {"DrawCircle", l_DrawCircle},
    {"DrawRectangle", l_DrawRectangle},
    {"DrawRectangleRec", l_DrawRectangleRec},
    {"DrawText", l_DrawText},
    {"IsWindowReady", l_IsWindowReady},
    {"MeasureText", l_MeasureText},
    {"ToggleFullscreen", l_ToggleFullscreen}, 
    {"SetWindowTitle", l_SetWindowTitle},
    {"IsWindowFullscreen", l_IsWindowFullscreen},
    {"GetScreenWidth", l_GetScreenWidth},
    {"GetScreenHeight", l_GetScreenHeight},
    {"SetWindowPosition", l_SetWindowPosition},
    {"SetWindowSize", l_SetWindowSize},
    {"MaximizeWindow", l_MaximizeWindow},
    {"MinimizeWindow", l_MinimizeWindow},
    {"GetTime", l_GetTime},
    {"SetRandomSeed", l_SetRandomSeed},
    {"GetRandomValue", l_GetRandomValue},
    {"TakeScreenshot", l_TakeScreenshot},
    // New input functions
    {"IsKeyReleased", l_IsKeyReleased},
    {"IsKeyUp", l_IsKeyUp},
    {"GetCharPressed", l_GetCharPressed},
    {"SetExitKey", l_SetExitKey},
    {"IsMouseButtonDown", l_IsMouseButtonDown},
    {"IsMouseButtonReleased", l_IsMouseButtonReleased},
    {"IsMouseButtonUp", l_IsMouseButtonUp},
    {"GetMouseX", l_GetMouseX},
    {"GetMouseY", l_GetMouseY},
    {"GetMouseWheelMove", l_GetMouseWheelMove},
    {"SetMousePosition", l_SetMousePosition},
    {"SetMouseCursor", l_SetMouseCursor},
    // New rshapes functions
    {"DrawCircleGradient", l_DrawCircleGradient},
    {"DrawCircleLines", l_DrawCircleLines},
    {"DrawRectangleLines", l_DrawRectangleLines},
    {"DrawRectangleGradientV", l_DrawRectangleGradientV},
    {"DrawTriangle", l_DrawTriangle},
    {"DrawTriangleLines", l_DrawTriangleLines},
    {"DrawPoly", l_DrawPoly},



    {NULL, NULL}
};

int luaopen_raylib(lua_State *L) {
    lua_newtable(L);
    luaL_setfuncs(L, raylib_funcs, 0);

    push_color(L, LIGHTGRAY); lua_setfield(L, -2, "LIGHTGRAY");
    push_color(L, GRAY);     lua_setfield(L, -2, "GRAY");
    push_color(L, DARKGRAY);     lua_setfield(L, -2, "DARKGRAY");
    push_color(L, YELLOW);   lua_setfield(L, -2, "YELLOW");
    push_color(L, GOLD);   lua_setfield(L, -2, "GOLD");
    push_color(L, ORANGE);   lua_setfield(L, -2, "ORANGE");
    push_color(L, PINK);   lua_setfield(L, -2, "PINK");
    push_color(L, RED);      lua_setfield(L, -2, "RED");
    push_color(L, MAROON);      lua_setfield(L, -2, "MAROON");
    push_color(L, GREEN);      lua_setfield(L, -2, "GREEN");
    push_color(L, LIME);      lua_setfield(L, -2, "LIME");
    push_color(L, DARKGREEN);      lua_setfield(L, -2, "DARKGREEN");
    push_color(L, SKYBLUE);      lua_setfield(L, -2, "SKYBLUE");
    push_color(L, BLUE);      lua_setfield(L, -2, "BLUE");
    push_color(L, DARKBLUE);      lua_setfield(L, -2, "DARKBLUE");
    push_color(L, PURPLE);   lua_setfield(L, -2, "PURPLE");
    push_color(L, VIOLET);   lua_setfield(L, -2, "VIOLET");
    push_color(L, DARKPURPLE);   lua_setfield(L, -2, "DARKPURPLE");
    push_color(L, BEIGE);   lua_setfield(L, -2, "BEIGE");
    push_color(L, BROWN);   lua_setfield(L, -2, "BROWN");
    push_color(L, DARKBROWN);   lua_setfield(L, -2, "DARKBROWN");

    push_color(L, WHITE); lua_setfield(L, -2, "WHITE");
    push_color(L, BLACK);    lua_setfield(L, -2, "BLACK");
    push_color(L, BLANK);    lua_setfield(L, -2, "BLANK");
    push_color(L, MAGENTA); lua_setfield(L, -2, "MAGENTA");
    push_color(L, RAYWHITE); lua_setfield(L, -2, "RAYWHITE");

    lua_pushinteger(L, KEY_NULL); lua_setfield(L, -2, "KEY_NULL");
    lua_pushinteger(L, KEY_APOSTROPHE); lua_setfield(L, -2, "KEY_APOSTROPHE");
    lua_pushinteger(L, KEY_COMMA); lua_setfield(L, -2, "KEY_COMMA");
    lua_pushinteger(L, KEY_MINUS); lua_setfield(L, -2, "KEY_MINUS");
    lua_pushinteger(L, KEY_PERIOD); lua_setfield(L, -2, "KEY_PERIOD");
    lua_pushinteger(L, KEY_SLASH); lua_setfield(L, -2, "KEY_SLASH");
    lua_pushinteger(L, KEY_ZERO); lua_setfield(L, -2, "KEY_ZERO");
    lua_pushinteger(L, KEY_ONE); lua_setfield(L, -2, "KEY_ONE");
    lua_pushinteger(L, KEY_TWO); lua_setfield(L, -2, "KEY_TWO");
    lua_pushinteger(L, KEY_THREE); lua_setfield(L, -2, "KEY_THREE");
    lua_pushinteger(L, KEY_FOUR); lua_setfield(L, -2, "KEY_FOUR");
    lua_pushinteger(L, KEY_FIVE); lua_setfield(L, -2, "KEY_FIVE");
    lua_pushinteger(L, KEY_SIX); lua_setfield(L, -2, "KEY_SIX");
    lua_pushinteger(L, KEY_SEVEN); lua_setfield(L, -2, "KEY_SEVEN");
    lua_pushinteger(L, KEY_EIGHT); lua_setfield(L, -2, "KEY_EIGHT");
    lua_pushinteger(L, KEY_NINE); lua_setfield(L, -2, "KEY_NINE");
    lua_pushinteger(L, KEY_SEMICOLON); lua_setfield(L, -2, "KEY_SEMICOLON");
    lua_pushinteger(L, KEY_EQUAL); lua_setfield(L, -2, "KEY_EQUAL");
    lua_pushinteger(L, KEY_A); lua_setfield(L, -2, "KEY_A");
    lua_pushinteger(L, KEY_B); lua_setfield(L, -2, "KEY_B");
    lua_pushinteger(L, KEY_C); lua_setfield(L, -2, "KEY_C");
    lua_pushinteger(L, KEY_D); lua_setfield(L, -2, "KEY_D");
    lua_pushinteger(L, KEY_E); lua_setfield(L, -2, "KEY_E");
    lua_pushinteger(L, KEY_F); lua_setfield(L, -2, "KEY_F");
    lua_pushinteger(L, KEY_G); lua_setfield(L, -2, "KEY_G");
    lua_pushinteger(L, KEY_H); lua_setfield(L, -2, "KEY_H");
    lua_pushinteger(L, KEY_I); lua_setfield(L, -2, "KEY_I");
    lua_pushinteger(L, KEY_J); lua_setfield(L, -2, "KEY_J");
    lua_pushinteger(L, KEY_K); lua_setfield(L, -2, "KEY_K");
    lua_pushinteger(L, KEY_L); lua_setfield(L, -2, "KEY_L");
    lua_pushinteger(L, KEY_M); lua_setfield(L, -2, "KEY_M");
    lua_pushinteger(L, KEY_N); lua_setfield(L, -2, "KEY_N");
    lua_pushinteger(L, KEY_P); lua_setfield(L, -2, "KEY_P");
    lua_pushinteger(L, KEY_Q); lua_setfield(L, -2, "KEY_Q");
    lua_pushinteger(L, KEY_R); lua_setfield(L, -2, "KEY_R");
    lua_pushinteger(L, KEY_S); lua_setfield(L, -2, "KEY_S");
    lua_pushinteger(L, KEY_T); lua_setfield(L, -2, "KEY_T");
    lua_pushinteger(L, KEY_U); lua_setfield(L, -2, "KEY_U");
    lua_pushinteger(L, KEY_V); lua_setfield(L, -2, "KEY_V");
    lua_pushinteger(L, KEY_W); lua_setfield(L, -2, "KEY_W");
    lua_pushinteger(L, KEY_X); lua_setfield(L, -2, "KEY_X");
    lua_pushinteger(L, KEY_Z); lua_setfield(L, -2, "KEY_Z");
    lua_pushinteger(L, KEY_LEFT_BRACKET); lua_setfield(L, -2, "KEY_LEFT_BRACKET");
    lua_pushinteger(L, KEY_BACKSLASH); lua_setfield(L, -2, "KEY_BACKSLASH");
    lua_pushinteger(L, KEY_RIGHT_BRACKET); lua_setfield(L, -2, "KEY_RIGHT_BRACKET");
    lua_pushinteger(L, KEY_GRAVE); lua_setfield(L, -2, "KEY_GRAVE");
    lua_pushinteger(L, KEY_SPACE); lua_setfield(L, -2, "KEY_SPACE");
    lua_pushinteger(L, KEY_ESCAPE); lua_setfield(L, -2, "KEY_ESCAPE");
    lua_pushinteger(L, KEY_ENTER); lua_setfield(L, -2, "KEY_ENTER");
    lua_pushinteger(L, KEY_TAB); lua_setfield(L, -2, "KEY_TAB");
    lua_pushinteger(L, KEY_BACKSPACE); lua_setfield(L, -2, "KEY_BACKSPACE");
    lua_pushinteger(L, KEY_INSERT); lua_setfield(L, -2, "KEY_INSERT");
    lua_pushinteger(L, KEY_DELETE); lua_setfield(L, -2, "KEY_DELETE");
    lua_pushinteger(L, KEY_RIGHT); lua_setfield(L, -2, "KEY_RIGHT");
    lua_pushinteger(L, KEY_LEFT); lua_setfield(L, -2, "KEY_LEFT");
    lua_pushinteger(L, KEY_DOWN); lua_setfield(L, -2, "KEY_DOWN");
    lua_pushinteger(L, KEY_UP); lua_setfield(L, -2, "KEY_UP");
    lua_pushinteger(L, KEY_PAGE_UP); lua_setfield(L, -2, "KEY_PAGE_UP");
    lua_pushinteger(L, KEY_PAGE_DOWN); lua_setfield(L, -2, "KEY_PAGE_DOWN");
    lua_pushinteger(L, KEY_HOME); lua_setfield(L, -2, "KEY_HOME");
    lua_pushinteger(L, KEY_END); lua_setfield(L, -2, "KEY_END");
    lua_pushinteger(L, KEY_CAPS_LOCK); lua_setfield(L, -2, "KEY_CAPS_LOCK");
    lua_pushinteger(L, KEY_SCROLL_LOCK); lua_setfield(L, -2, "KEY_SCROLL_LOCK");
    lua_pushinteger(L, KEY_NUM_LOCK); lua_setfield(L, -2, "KEY_NUM_LOCK");
    lua_pushinteger(L, KEY_PRINT_SCREEN); lua_setfield(L, -2, "KEY_PRINT_SCREEN");
    lua_pushinteger(L, KEY_PAUSE); lua_setfield(L, -2, "KEY_PAUSE");
    lua_pushinteger(L, KEY_F1); lua_setfield(L, -2, "KEY_F1");
    lua_pushinteger(L, KEY_F2); lua_setfield(L, -2, "KEY_F2");
    lua_pushinteger(L, KEY_F3); lua_setfield(L, -2, "KEY_F3");
    lua_pushinteger(L, KEY_F4); lua_setfield(L, -2, "KEY_F4");
    lua_pushinteger(L, KEY_F5); lua_setfield(L, -2, "KEY_F5");
    lua_pushinteger(L, KEY_F6); lua_setfield(L, -2, "KEY_F6");
    lua_pushinteger(L, KEY_F7); lua_setfield(L, -2, "KEY_F7");
    lua_pushinteger(L, KEY_F8); lua_setfield(L, -2, "KEY_F8");
    lua_pushinteger(L, KEY_F9); lua_setfield(L, -2, "KEY_F9");
    lua_pushinteger(L, KEY_F10); lua_setfield(L, -2, "KEY_F10");
    lua_pushinteger(L, KEY_F11); lua_setfield(L, -2, "KEY_F11");
    lua_pushinteger(L, KEY_F12); lua_setfield(L, -2, "KEY_F12");
    lua_pushinteger(L, KEY_LEFT_SHIFT); lua_setfield(L, -2, "KEY_LEFT_SHIFT");
    lua_pushinteger(L, KEY_LEFT_CONTROL); lua_setfield(L, -2, "KEY_LEFT_CONTROL");
    lua_pushinteger(L, KEY_LEFT_ALT); lua_setfield(L, -2, "KEY_LEFT_ALT");
    lua_pushinteger(L, KEY_LEFT_SUPER); lua_setfield(L, -2, "KEY_LEFT_SUPER");
    lua_pushinteger(L, KEY_RIGHT_SHIFT); lua_setfield(L, -2, "KEY_RIGHT_SHIFT");
    lua_pushinteger(L, KEY_RIGHT_CONTROL); lua_setfield(L, -2, "KEY_RIGHT_CONTROL");
    lua_pushinteger(L, KEY_RIGHT_ALT); lua_setfield(L, -2, "KEY_RIGHT_ALT");
    lua_pushinteger(L, KEY_RIGHT_SUPER); lua_setfield(L, -2, "KEY_RIGHT_SUPER");
    lua_pushinteger(L, KEY_KB_MENU); lua_setfield(L, -2, "KEY_KB_MENU");
    lua_pushinteger(L, KEY_KP_0); lua_setfield(L, -2, "KEY_KP_0");
    lua_pushinteger(L, KEY_KP_1); lua_setfield(L, -2, "KEY_KP_1");
    lua_pushinteger(L, KEY_KP_2); lua_setfield(L, -2, "KEY_KP_2");
    lua_pushinteger(L, KEY_KP_3); lua_setfield(L, -2, "KEY_KP_3");
    lua_pushinteger(L, KEY_KP_4); lua_setfield(L, -2, "KEY_KP_4");
    lua_pushinteger(L, KEY_KP_5); lua_setfield(L, -2, "KEY_KP_5");
    lua_pushinteger(L, KEY_KP_6); lua_setfield(L, -2, "KEY_KP_6");
    lua_pushinteger(L, KEY_KP_7); lua_setfield(L, -2, "KEY_KP_7");
    lua_pushinteger(L, KEY_KP_8); lua_setfield(L, -2, "KEY_KP_8");
    lua_pushinteger(L, KEY_KP_9); lua_setfield(L, -2, "KEY_KP_9");
    lua_pushinteger(L, KEY_KP_DECIMAL); lua_setfield(L, -2, "KEY_KP_DECIMAL");
    lua_pushinteger(L, KEY_KP_DIVIDE); lua_setfield(L, -2, "KEY_KP_DIVIDE");
    lua_pushinteger(L, KEY_KP_MULTIPLY); lua_setfield(L, -2, "KEY_KP_MULTIPLY");
    lua_pushinteger(L, KEY_KP_SUBTRACT); lua_setfield(L, -2, "KEY_KP_SUBTRACT");
    lua_pushinteger(L, KEY_KP_ADD); lua_setfield(L, -2, "KEY_KP_ADD");
    lua_pushinteger(L, KEY_KP_ENTER); lua_setfield(L, -2, "KEY_KP_ENTER");
    lua_pushinteger(L, KEY_KP_EQUAL); lua_setfield(L, -2, "KEY_KP_EQUAL");
    lua_pushinteger(L, KEY_BACK); lua_setfield(L, -2, "KEY_BACK");
    lua_pushinteger(L, KEY_MENU); lua_setfield(L, -2, "KEY_MENU");
    lua_pushinteger(L, KEY_VOLUME_UP); lua_setfield(L, -2, "KEY_VOLUME_UP");
    lua_pushinteger(L, KEY_VOLUME_DOWN); lua_setfield(L, -2, "KEY_VOLUME_DOWN");
    
    lua_pushinteger(L, MOUSE_BUTTON_LEFT); lua_setfield(L, -2, "MOUSE_BUTTON_LEFT");
    lua_pushinteger(L, MOUSE_BUTTON_RIGHT); lua_setfield(L, -2, "MOUSE_BUTTON_RIGHT");

    lua_pushinteger(L, MOUSE_CURSOR_DEFAULT); lua_setfield(L, -2, "MOUSE_CURSOR_DEFAULT");
    lua_pushinteger(L, MOUSE_CURSOR_ARROW); lua_setfield(L, -2, "MOUSE_CURSOR_ARROW");
    lua_pushinteger(L, MOUSE_CURSOR_IBEAM); lua_setfield(L, -2, "MOUSE_CURSOR_IBEAM");
    lua_pushinteger(L, MOUSE_CURSOR_CROSSHAIR); lua_setfield(L, -2, "MOUSE_CURSOR_CROSSHAIR");
    lua_pushinteger(L, MOUSE_CURSOR_POINTING_HAND); lua_setfield(L, -2, "MOUSE_CURSOR_POINTING_HAND");
    lua_pushinteger(L, MOUSE_CURSOR_RESIZE_EW); lua_setfield(L, -2, "MOUSE_CURSOR_RESIZE_EW");
    lua_pushinteger(L, MOUSE_CURSOR_RESIZE_NS); lua_setfield(L, -2, "MOUSE_CURSOR_RESIZE_NS");

    return 1;
}