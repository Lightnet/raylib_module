#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "flecs.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
// https://stackoverflow.com/questions/76491017/disable-raylib-iog-messages
// 
// Custom logging function
void CustomLog(int msgType, const char *text, va_list args){
  char timeStr[64] = { 0 };
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
  printf("[%s] ", timeStr);
  switch (msgType)
  {
      case LOG_INFO: printf("[INFO] : "); break;
      case LOG_ERROR: printf("[ERROR]: "); break;
      case LOG_WARNING: printf("[WARN] : "); break;
      case LOG_DEBUG: printf("[DEBUG]: "); break;
      default: break;
  }
  vprintf(text, args);
  printf("\n");
}

// Main function: entry point for execution
int main() {
  // Set custom logger
  //SetTraceLogCallback(CustomLog);
  SetTraceLogLevel(LOG_ERROR); 
    const int screenWidth = 800;
    const int screenHeight = 450;
    // Writing print statement to print hello world
    printf("Hello World");

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();

          ClearBackground(RAYWHITE);

          DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow(); 

    return 0;
}