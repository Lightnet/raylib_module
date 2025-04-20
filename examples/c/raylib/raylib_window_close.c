// normal test builds.
#include "raylib.h"
#include <stdio.h>                  // Required for: fopen(), fclose(), fputc(), fwrite(), printf(), fprintf(), funopen()

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    printf("init...\n");

    InitWindow(screenWidth, screenHeight, "raylib [core] example - custom logging");
    //remove escape key for close
    SetExitKey(KEY_NULL);  // Set no key to close window automatically

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Check out the console output to see the custom logger in action!", 60, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}