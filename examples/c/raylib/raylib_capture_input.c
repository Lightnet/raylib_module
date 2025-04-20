#include "raylib.h"

int main(void)
{
    // Initialize window
    InitWindow(800, 600, "Mouse Capture Example");
    SetTargetFPS(60);

    bool isMouseCaptured = false;

    while (!WindowShouldClose())
    {
        // Capture mouse on any key or mouse button press
        if (!isMouseCaptured && (IsKeyPressed(KEY_ANY) || IsMouseButtonPressed(MOUSE_BUTTON_ANY)))
        {
            HideCursor();
            DisableCursor();  // Locks mouse to window
            isMouseCaptured = true;
        }

        // Release mouse on Escape key
        if (isMouseCaptured && IsKeyPressed(KEY_ESCAPE))
        {
            EnableCursor();  // Release mouse
            ShowCursor();
            isMouseCaptured = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw mouse position and capture status
        Vector2 mousePos = GetMousePosition();
        DrawText(TextFormat("Mouse Position: %.0f, %.0f", mousePos.x, mousePos.y), 10, 10, 20, BLACK);
        DrawText(isMouseCaptured ? "Mouse Captured (Press ESC to release)" : "Mouse Free (Press any key/mouse to capture)", 10, 40, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}