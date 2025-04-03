#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"
#include "raymath.h"
#define ENET_IMPLEMENTATION
#include <enet.h>

// Object entity structure
typedef enum { OBJ_BOX, OBJ_SPHERE, OBJ_MESH } ObjectType;

typedef struct {
    Vector3 size;
    Vector3 position;
    ObjectType objType;
    Color color;
} ObjectEntity;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Voxel Collision Demo");

    // Define the camera (first-person style)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    // Player entity
    ObjectEntity player = {
        .size = {1.0f, 1.0f, 1.0f},
        .position = {0.0f, 0.5f, 0.0f},
        .objType = OBJ_BOX,
        .color = GREEN
    };

    // Floor entity
    ObjectEntity floor = {
        .size = {10.0f, 0.2f, 10.0f},
        .position = {0.0f, -0.1f, 0.0f},
        .objType = OBJ_BOX,
        .color = GRAY
    };

    // Test enemy entity
    ObjectEntity enemy = {
        .size = {1.0f, 1.0f, 1.0f},
        .position = {2.0f, 0.5f, 2.0f},
        .objType = OBJ_BOX,
        .color = RED
    };

    float playerVelocityY = 0.0f;
    const float gravity = -20.0f;
    const float jumpForce = 8.0f;
    bool mouseCaptured = false;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        // Mouse capture toggle
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !mouseCaptured) {
            mouseCaptured = true;
            DisableCursor();
        }
        if (IsKeyPressed(KEY_ESCAPE) && mouseCaptured) {
            mouseCaptured = false;
            EnableCursor();
        }

        // Camera rotation (when mouse captured)
        if (mouseCaptured) {
            Vector2 mouseDelta = GetMouseDelta();
            //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            
            // Manual rotation control for correct order (yaw then pitch)
            float yaw = -mouseDelta.x * 0.003f;
            float pitch = -mouseDelta.y * 0.003f;
            
            // Limit pitch
            float currentPitch = atan2f(camera.target.y - camera.position.y, 
                                      Vector3Length((Vector3){camera.target.x - camera.position.x, 0, camera.target.z - camera.position.z}));
            pitch = currentPitch + pitch;
            if (pitch > 1.5f) pitch = 1.5f;
            if (pitch < -1.5f) pitch = -1.5f;

            camera.target = Vector3RotateByAxisAngle(
                Vector3Subtract(camera.target, camera.position),
                (Vector3){0, 1, 0},
                yaw
            );
            camera.target = Vector3Add(camera.target, camera.position);
            
            Vector3 right = Vector3CrossProduct(
                Vector3Subtract(camera.target, camera.position),
                camera.up
            );
            camera.target = Vector3RotateByAxisAngle(
                Vector3Subtract(camera.target, camera.position),
                right,
                pitch
            );
            camera.target = Vector3Add(camera.target, camera.position);
        }

        // Player movement
        Vector3 moveDir = {0};
        if (mouseCaptured) {
            if (IsKeyDown(KEY_W)) moveDir.z = 1.0f;
            if (IsKeyDown(KEY_S)) moveDir.z = -1.0f;
            if (IsKeyDown(KEY_A)) moveDir.x = -1.0f;
            if (IsKeyDown(KEY_D)) moveDir.x = 1.0f;
            
            // Normalize movement direction and apply camera orientation
            if (Vector3Length(moveDir) > 0) {
                moveDir = Vector3Normalize(moveDir);
                Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
                Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
                forward.y = 0; // Keep movement on horizontal plane
                forward = Vector3Normalize(forward);
                
                Vector3 movement = Vector3Add(
                    Vector3Scale(forward, moveDir.z),
                    Vector3Scale(right, moveDir.x)
                );
                player.position.x += movement.x * 5.0f * deltaTime;
                player.position.z += movement.z * 5.0f * deltaTime;
            }
        }

        // Gravity and jumping
        playerVelocityY += gravity * deltaTime;
        if (IsKeyPressed(KEY_SPACE) && player.position.y <= 0.5f) {
            playerVelocityY = jumpForce;
        }
        player.position.y += playerVelocityY * deltaTime;

        // Floor collision
        BoundingBox playerBB = {
            (Vector3){player.position.x - player.size.x/2, player.position.y - player.size.y/2, player.position.z - player.size.z/2},
            (Vector3){player.position.x + player.size.x/2, player.position.y + player.size.y/2, player.position.z + player.size.z/2}
        };
        BoundingBox floorBB = {
            (Vector3){floor.position.x - floor.size.x/2, floor.position.y - floor.size.y/2, floor.position.z - floor.size.z/2},
            (Vector3){floor.position.x + floor.size.x/2, floor.position.y + floor.size.y/2, floor.position.z + floor.size.z/2}
        };
        
        if (CheckCollisionBoxes(playerBB, floorBB)) {
            player.position.y = floor.position.y + floor.size.y/2 + player.size.y/2;
            playerVelocityY = 0.0f;
        }

        // Enemy collision
        BoundingBox enemyBB = {
            (Vector3){enemy.position.x - enemy.size.x/2, enemy.position.y - enemy.size.y/2, enemy.position.z - enemy.size.z/2},
            (Vector3){enemy.position.x + enemy.size.x/2, enemy.position.y + enemy.size.y/2, enemy.position.z + enemy.size.z/2}
        };
        player.color = CheckCollisionBoxes(playerBB, enemyBB) ? RED : GREEN;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawCubeV(player.position, player.size, player.color);
                DrawCubeWiresV(player.position, player.size, DARKGRAY);
                DrawCubeV(floor.position, floor.size, floor.color);
                DrawCubeV(enemy.position, enemy.size, enemy.color);
                DrawCubeWiresV(enemy.position, enemy.size, DARKGRAY);
            EndMode3D();

            DrawText("Left click to capture mouse, ESC to release", 10, 10, 20, GRAY);
            DrawText("WASD to move, SPACE to jump", 10, 30, 20, GRAY);
            DrawFPS(10, 50);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}