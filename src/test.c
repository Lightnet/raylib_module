#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND


// Header file for input output functions
#include <stdio.h>
#include "flecs.h"
#include <raylib.h>

// Main function: entry point for execution
int main() {
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