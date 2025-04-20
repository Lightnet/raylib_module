#include "raylib.h"
#include "rcamera.h"
#include "raymath.h" // For raymath functions
#include <math.h>    // For standard math functions (cosf, sinf)

// Define custom camera modes with F_ prefix to avoid conflicts
typedef enum {
    F_CAMERA_FREE,   // Free camera (WASD, mouse, Space/Ctrl)
    F_CAMERA_PLAYER, // First-person player camera
    F_CAMERA_DEBUG   // Debug camera (orbit with mouse)
} FCameraMode;

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Raylib 5.5 - Custom Camera Mode Toggle");

    // Camera setup
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 10.0f }; // Initial position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };    // Look at origin
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        // Up vector
    camera.fovy = 60.0f;                              // Field of view
    camera.projection = CAMERA_PERSPECTIVE;

    // Player position (for PLAYER and DEBUG modes)
    Vector3 playerPos = { 0.0f, 1.0f, 0.0f }; // Player at origin, raised

    // Debug camera settings
    float debugDistance = 10.0f; // Distance from target
    float debugYaw = 0.0f;       // Yaw angle
    float debugPitch = 0.0f;     // Pitch angle

    // Current camera mode
    FCameraMode currentMode = F_CAMERA_PLAYER;

    SetTargetFPS(60);
    DisableCursor(); // Lock cursor for camera control

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        // Toggle camera mode with Tab
        if (IsKeyPressed(KEY_TAB))
        {
            currentMode = (FCameraMode)((currentMode + 1) % 3); // Cycle through modes
            // Reset camera settings
            switch (currentMode)
            {
                case F_CAMERA_FREE:
                    camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
                    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                    break;
                case F_CAMERA_PLAYER:
                    camera.position = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z };
                    camera.target = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z - 1.0f };
                    break;
                case F_CAMERA_DEBUG:
                    debugDistance = 10.0f;
                    debugYaw = 0.0f;
                    debugPitch = 0.0f;
                    camera.position = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z + debugDistance };
                    camera.target = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z };
                    break;
            }
        }

        // Update camera based on mode
        float deltaTime = GetFrameTime();
        float moveSpeed = 5.0f * deltaTime; // Movement speed (units/second)
        float turnSpeed = 90.0f * deltaTime; // Rotation speed (degrees/second)

        switch (currentMode)
        {
            case F_CAMERA_FREE:
            {
                // Free camera: WASD, mouse, Space/Ctrl
                if (IsKeyDown(KEY_W)) CameraMoveForward(&camera, moveSpeed, true);
                if (IsKeyDown(KEY_S)) CameraMoveForward(&camera, -moveSpeed, true);
                if (IsKeyDown(KEY_A)) CameraMoveRight(&camera, -moveSpeed, true);
                if (IsKeyDown(KEY_D)) CameraMoveRight(&camera, moveSpeed, true);
                if (IsKeyDown(KEY_SPACE)) CameraMoveUp(&camera, moveSpeed);
                if (IsKeyDown(KEY_LEFT_CONTROL)) CameraMoveUp(&camera, -moveSpeed);
                Vector2 mouseDelta = GetMouseDelta();
                CameraYaw(&camera, -mouseDelta.x * 0.05f * turnSpeed, true);
                CameraPitch(&camera, -mouseDelta.y * 0.05f * turnSpeed, true, true, false);
                break;
            }
            case F_CAMERA_PLAYER:
            {
                // Player camera: WASD to move player, mouse to look
                if (IsKeyDown(KEY_W)) playerPos.z -= moveSpeed;
                if (IsKeyDown(KEY_S)) playerPos.z += moveSpeed;
                if (IsKeyDown(KEY_A)) playerPos.x -= moveSpeed;
                if (IsKeyDown(KEY_D)) playerPos.x += moveSpeed;
                camera.position = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z };
                Vector2 mouseDelta = GetMouseDelta();
                CameraYaw(&camera, -mouseDelta.x * 0.05f * turnSpeed, true);
                CameraPitch(&camera, -mouseDelta.y * 0.05f * turnSpeed, true, true, false);
                break;
            }
            case F_CAMERA_DEBUG:
            {
                // Debug camera: orbit with mouse drag, scroll to zoom
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    Vector2 mouseDelta = GetMouseDelta();
                    debugYaw -= mouseDelta.x * 0.05f * turnSpeed;
                    debugPitch -= mouseDelta.y * 0.05f * turnSpeed;
                    debugPitch = Clamp(debugPitch, -PI / 2.0f, PI / 2.0f); // Use raymath Clamp
                }
                debugDistance -= GetMouseWheelMove() * 0.5f;
                debugDistance = Clamp(debugDistance, 2.0f, 50.0f); // Prevent too close/far
                // Update camera position (orbit around player)
                camera.position.x = playerPos.x + debugDistance * cosf(debugYaw) * cosf(debugPitch);
                camera.position.y = playerPos.y + 1.0f + debugDistance * sinf(debugPitch);
                camera.position.z = playerPos.z + debugDistance * sinf(debugYaw) * cosf(debugPitch);
                camera.target = (Vector3){ playerPos.x, playerPos.y + 1.0f, playerPos.z };
                break;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        // Draw scene
        DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY);
        DrawCube(playerPos, 1.0f, 1.0f, 1.0f, RED); // Player
        EndMode3D();

        // Display mode and controls
        const char* modeText = (currentMode == F_CAMERA_FREE) ? "FREE" :
                               (currentMode == F_CAMERA_PLAYER) ? "PLAYER" : "DEBUG";
        DrawText(TextFormat("Mode: %s (TAB to toggle)", modeText), 10, 10, 20, DARKGRAY);
        DrawText("Free: WASD, Mouse, Space/Ctrl | Player: WASD, Mouse | Debug: Mouse drag, Scroll", 10, 40, 10, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}