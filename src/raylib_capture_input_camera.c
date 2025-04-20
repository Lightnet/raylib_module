#include "raylib.h"
#include "raymath.h"  // For vector and matrix operations

// Function to check if any common key is pressed [input]
bool IsAnyKeyPressed(void)
{
    // Check common keys (letters, numbers, space, etc.) [input]
    int keys[] = {
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
        KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
        KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX,
        KEY_SEVEN, KEY_EIGHT, KEY_NINE,
        KEY_SPACE, KEY_ENTER, KEY_TAB
    };
    int keyCount = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < keyCount; i++)
    {
        if (IsKeyPressed(keys[i])) return true;
    }
    return false;
}

// Function to check if any mouse button is pressed [input]
bool IsAnyMouseButtonPressed(void)
{
    // Check all defined mouse buttons [input]
    int buttons[] = {
        MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_SIDE, MOUSE_BUTTON_EXTRA, MOUSE_BUTTON_FORWARD, MOUSE_BUTTON_BACK
    };
    int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

    for (int i = 0; i < buttonCount; i++)
    {
        if (IsMouseButtonPressed(buttons[i])) return true;
    }
    return false;
}

int main(void)
{
    // Window setup [core]
    InitWindow(800, 600, "Camera Movement with Mouse Capture");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  // Disable ESCAPE closing window [core]

    // Camera setup [camera]
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.0f, 0.0f };  // Start at origin, eye height
    camera.target = (Vector3){ 0.0f, 1.0f, 1.0f };    // Look forward
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        // Up vector
    camera.fovy = 60.0f;                              // Field of view
    camera.projection = CAMERA_PERSPECTIVE;           // Perspective projection

    // Mouse capture state
    bool isMouseCaptured = false;
    float yaw = 0.0f, pitch = 0.0f;  // Camera rotation angles
    const float mouseSensitivity = 0.005f;  // Mouse movement sensitivity
    const float moveSpeed = 0.1f;   // Camera movement speed

    while (!WindowShouldClose())  // Loop until close button clicked [core]
    {
        // Capture mouse on any key or mouse button [input]
        if (!isMouseCaptured && (IsAnyKeyPressed() || IsAnyMouseButtonPressed()))
        {
            // Exclude ESCAPE key from triggering capture
            if (!IsKeyPressed(KEY_ESCAPE))
            {
                HideCursor();          // Hide cursor [core]
                DisableCursor();       // Lock mouse to window [core]
                isMouseCaptured = true;
            }
        }

        // Release mouse on ESCAPE [input]
        if (isMouseCaptured && IsKeyPressed(KEY_ESCAPE))
        {
            EnableCursor();        // Unlock mouse [core]
            ShowCursor();          // Show cursor [core]
            isMouseCaptured = false;
        }

        // Camera rotation when mouse captured [input, raymath]
        if (isMouseCaptured)
        {
            Vector2 mouseDelta = GetMouseDelta();  // Mouse movement [input]
            yaw -= mouseDelta.x * mouseSensitivity;
            pitch -= mouseDelta.y * mouseSensitivity;
            pitch = Clamp(pitch, -PI/2.0f + 0.1f, PI/2.0f - 0.1f);  // Limit pitch [raymath]
            
            // Update camera target based on yaw and pitch [raymath]
            camera.target = Vector3Add(camera.position, (Vector3){
                cosf(pitch) * sinf(yaw),
                sinf(pitch),
                cosf(pitch) * cosf(yaw)
            });
        }

        // Camera movement based on direction [input, raymath]
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));  // Forward vector [raymath]
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));  // Right vector [raymath]
        Vector3 up = camera.up;  // Global up vector (0, 1, 0) [raymath]

        // Forward/Backward (W/S)
        if (IsKeyDown(KEY_W))  // Move forward [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, moveSpeed));  // [raymath]
        if (IsKeyDown(KEY_S))  // Move backward [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, -moveSpeed)); // [raymath]

        // Left/Right (A/D)
        if (IsKeyDown(KEY_A))  // Move left [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(right, -moveSpeed));  // [raymath]
        if (IsKeyDown(KEY_D))  // Move right [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(right, moveSpeed));   // [raymath]

        // Up/Down (Space/Shift)
        if (IsKeyDown(KEY_SPACE))  // Move up [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(up, moveSpeed));      // [raymath]
        if (IsKeyDown(KEY_LEFT_SHIFT))  // Move down [input]
            camera.position = Vector3Add(camera.position, Vector3Scale(up, -moveSpeed));     // [raymath]

        // Update camera target after movement
        camera.target = Vector3Add(camera.position, forward);

        // Draw scene [core, camera, shapes]
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(camera);  // Start 3D mode [camera]
                DrawGrid(10, 1.0f);  // Draw ground grid [shapes]
                DrawCube((Vector3){ 2.0f, 0.5f, 2.0f }, 1.0f, 1.0f, 1.0f, RED);  // Draw cube [shapes]
            EndMode3D();
            
            // Draw UI text [text]
            DrawText(isMouseCaptured ? "Mouse Captured (ESC to release)" : "Mouse Free (Any key/mouse to capture)", 10, 10, 20, BLACK);
            DrawText("W/S: Forward/Back, A/D: Left/Right, Space/Shift: Up/Down, Mouse: Look, Close button: Exit", 10, 40, 20, BLACK);
            DrawFPS(10, 70);  // Draw FPS [core]
        EndDrawing();
    }

    CloseWindow();  // Cleanup [core]
    return 0;
}