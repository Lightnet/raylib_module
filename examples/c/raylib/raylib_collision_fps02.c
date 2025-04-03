#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"
#include "raymath.h"
#define ENET_IMPLEMENTATION
#include <enet.h>
#include <stdio.h>

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

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.5f, 0.0f };
    camera.target = (Vector3){ 0.0f, 1.5f, -1.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    ObjectEntity player = {
        .size = {1.0f, 1.0f, 1.0f},
        .position = {0.0f, 0.5f, 0.0f},
        .objType = OBJ_BOX,
        .color = GREEN
    };

    #define FLOOR_SIZE 10
    ObjectEntity floor[FLOOR_SIZE * FLOOR_SIZE];
    for (int x = 0; x < FLOOR_SIZE; x++) {
        for (int z = 0; z < FLOOR_SIZE; z++) {
            int index = x * FLOOR_SIZE + z;
            floor[index] = (ObjectEntity){
                .size = {1.0f, 1.0f, 1.0f},
                .position = {(float)x - FLOOR_SIZE/2 + 0.5f, 0.0f, (float)z - FLOOR_SIZE/2 + 0.5f},
                .objType = OBJ_BOX,
                .color = GRAY
            };
        }
    }

    ObjectEntity enemy = {
        .size = {1.0f, 1.0f, 1.0f},
        .position = {2.0f, 1.0f, 2.0f},
        .objType = OBJ_BOX,
        .color = RED
    };

    float playerVelocityY = 0.0f;
    const float gravity = -20.0f;
    const float jumpForce = 8.0f;
    bool mouseCaptured = false;
    
    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;
    const float cameraHeightOffset = 0.5f;

    SetTargetFPS(60);
    DisableCursor();
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

        // Camera rotation
        if (mouseCaptured) {
            Vector2 mouseDelta = GetMouseDelta();
            cameraYaw -= mouseDelta.x * 0.003f;
            cameraPitch -= mouseDelta.y * 0.003f;
            if (cameraPitch > 1.5f) cameraPitch = 1.5f;
            if (cameraPitch < -1.5f) cameraPitch = -1.5f;

            Vector3 direction = {
                cosf(cameraPitch) * sinf(cameraYaw),
                sinf(cameraPitch),
                cosf(cameraPitch) * cosf(cameraYaw)
            };
            direction = Vector3Normalize(direction);
            
            camera.position = (Vector3){
                player.position.x,
                player.position.y + cameraHeightOffset,
                player.position.z
            };
            camera.target = Vector3Add(camera.position, direction);
        }

        // Player movement
        Vector3 moveDir = {0};
        if (mouseCaptured) {
            if (IsKeyDown(KEY_W)) moveDir.z = 1.0f;
            if (IsKeyDown(KEY_S)) moveDir.z = -1.0f;
            if (IsKeyDown(KEY_A)) moveDir.x = -1.0f;
            if (IsKeyDown(KEY_D)) moveDir.x = 1.0f;
            
            if (Vector3Length(moveDir) > 0) {
                moveDir = Vector3Normalize(moveDir);
                Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
                Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
                forward.y = 0;
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
        if (IsKeyPressed(KEY_SPACE) && player.position.y <= 1.0f) {
            playerVelocityY = jumpForce;
        }
        player.position.y += playerVelocityY * deltaTime;

        // Collision detection with floor blocks
        Vector3 playerMin = Vector3Subtract(player.position, Vector3Scale(player.size, 0.5f));
        Vector3 playerMax = Vector3Add(player.position, Vector3Scale(player.size, 0.5f));
        
        bool onGround = false;
        for (int i = 0; i < FLOOR_SIZE * FLOOR_SIZE; i++) {
            Vector3 blockMin = Vector3Subtract(floor[i].position, Vector3Scale(floor[i].size, 0.5f));
            Vector3 blockMax = Vector3Add(floor[i].position, Vector3Scale(floor[i].size, 0.5f));
            
            // Check collision using AABB (Axis-Aligned Bounding Box)
            if (playerMin.x < blockMax.x && playerMax.x > blockMin.x &&
                playerMin.y < blockMax.y && playerMax.y > blockMin.y &&
                playerMin.z < blockMax.z && playerMax.z > blockMin.z) {
                // Resolve collision - push player up to top of block
                if (playerVelocityY < 0) {  // Only when falling
                    player.position.y = blockMax.y + player.size.y/2;
                    playerVelocityY = 0.0f;
                    onGround = true;
                }
            }
        }

        // Enemy collision
        Vector3 enemyMin = Vector3Subtract(enemy.position, Vector3Scale(enemy.size, 0.5f));
        Vector3 enemyMax = Vector3Add(enemy.position, Vector3Scale(enemy.size, 0.5f));
        if (playerMin.x < enemyMax.x && playerMax.x > enemyMin.x &&
            playerMin.y < enemyMax.y && playerMax.y > enemyMin.y &&
            playerMin.z < enemyMax.z && playerMax.z > enemyMin.z) {
            player.color = RED;
        } else {
            player.color = GREEN;
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawCubeV(player.position, player.size, player.color);
                DrawCubeWiresV(player.position, player.size, DARKGRAY);
                
                // Draw floor cubes with slight separation to show individual blocks
                for (int i = 0; i < FLOOR_SIZE * FLOOR_SIZE; i++) {
                    Vector3 drawSize = Vector3Scale(floor[i].size, 0.98f); // Slightly smaller to show gaps
                    DrawCubeV(floor[i].position, drawSize, floor[i].color);
                    DrawCubeWiresV(floor[i].position, floor[i].size, DARKGRAY);
                }
                
                DrawCubeV(enemy.position, enemy.size, enemy.color);
                DrawCubeWiresV(enemy.position, enemy.size, DARKGRAY);
            EndMode3D();

            // Display player position
            char posText[64];
            sprintf(posText, "Position: X: %.2f Y: %.2f Z: %.2f", 
                   player.position.x, player.position.y, player.position.z);
            DrawText(posText, 10, 70, 20, GRAY);
            
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