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
typedef enum { STATE_GROUNDED, STATE_JUMPING, STATE_FALLING } PlayerState;

typedef struct {
    Vector3 size;
    Vector3 position;
    ObjectType objType;
    Color color;
    PlayerState state;
} ObjectEntity;

// Custom AABB collision check
bool CCheckCollisionBoxes(ObjectEntity a, ObjectEntity b) {
    BoundingBox boxA = {
        Vector3Subtract(a.position, Vector3Scale(a.size, 0.5f)),
        Vector3Add(a.position, Vector3Scale(a.size, 0.5f))
    };
    BoundingBox boxB = {
        Vector3Subtract(b.position, Vector3Scale(b.size, 0.5f)),
        Vector3Add(b.position, Vector3Scale(b.size, 0.5f))
    };
    return CheckCollisionBoxes(boxA, boxB);
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Voxel Collision Demo");

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, -1.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    ObjectEntity player = {
        .size = {1.0f, 1.8f, 1.0f},  // Height 1.8
        .position = {0.0f, 1.9f, 0.0f},  // Starting y adjusted for new height
        .objType = OBJ_BOX,
        .color = GREEN,
        .state = STATE_GROUNDED
    };

    #define FLOOR_SIZE 10
    #define MAX_BLOCKS (FLOOR_SIZE * FLOOR_SIZE + 3)
    ObjectEntity blocks[MAX_BLOCKS];
    int blockCount = 0;

    // Floor
    for (int x = 0; x < FLOOR_SIZE; x++) {
        for (int z = 0; z < FLOOR_SIZE; z++) {
            blocks[blockCount] = (ObjectEntity){
                .size = {1.0f, 1.0f, 1.0f},
                .position = {(float)x - FLOOR_SIZE/2 + 0.5f, 0.0f, (float)z - FLOOR_SIZE/2 + 0.5f},
                .objType = OBJ_BOX,
                .color = GRAY
            };
            blockCount++;
        }
    }

    // Enemy
    blocks[blockCount] = (ObjectEntity){
        .size = {1.0f, 1.0f, 1.0f},
        .position = {2.0f, 1.0f, 2.0f},
        .objType = OBJ_BOX,
        .color = RED
    };
    blockCount++;

    // Stacked Wall
    blocks[blockCount] = (ObjectEntity){
        .size = {1.0f, 1.0f, 1.0f},
        .position = {1.5f, 1.0f, 0.0f},  // Bottom block
        .objType = OBJ_BOX,
        .color = BLUE
    };
    blockCount++;

    blocks[blockCount] = (ObjectEntity){
        .size = {1.0f, 1.0f, 1.0f},
        .position = {1.5f, 2.0f, 0.0f},  // Top block
        .objType = OBJ_BOX,
        .color = BLUE
    };
    blockCount++;

    float playerVelocityY = 0.0f;
    const float gravity = -20.0f;
    const float jumpForce = 8.0f;
    bool mouseCaptured = false;
    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;
    const float cameraHeightOffset = 0.7f;  // Lowered from 0.9f to avoid clipping

    SetTargetFPS(60);
    DisableCursor();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !mouseCaptured) {
            mouseCaptured = true;
            DisableCursor();
        }
        if (IsKeyPressed(KEY_ESCAPE) && mouseCaptured) {
            mouseCaptured = false;
            EnableCursor();
        }

        if (IsKeyPressed(KEY_R)) {
            player.position = (Vector3){0.0f, 2.9f, 0.0f};  // Reset height for 1.8
            playerVelocityY = 0.0f;
            cameraYaw = 0.0f;
            cameraPitch = 0.0f;
            player.state = STATE_GROUNDED;
            player.color = GREEN;
        }

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
        Vector3 velocity = {0};
        if (mouseCaptured) {
            Vector3 moveDir = {0};
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
                
                velocity = Vector3Add(
                    Vector3Scale(forward, moveDir.z),
                    Vector3Scale(right, moveDir.x)
                );
                velocity = Vector3Scale(velocity, 5.0f);
            }
        }

        // Apply gravity and jumping
        if (player.state != STATE_GROUNDED) {
            playerVelocityY += gravity * deltaTime;
        }
        if (IsKeyPressed(KEY_SPACE) && player.state == STATE_GROUNDED) {
            player.state = STATE_JUMPING;
            playerVelocityY = jumpForce;
        }
        velocity.y = playerVelocityY;

        // Calculate new position
        Vector3 newPosition = Vector3Add(player.position, Vector3Scale(velocity, deltaTime));
        ObjectEntity tempPlayer = player;

        // Wall collision check (forward, back, left, right)
        float wallCheckDistance = 0.1f;
        Vector3 wallCheckPositions[4] = {
            {player.position.x, player.position.y, player.position.z + wallCheckDistance},  // Forward
            {player.position.x, player.position.y, player.position.z - wallCheckDistance},  // Back
            {player.position.x - wallCheckDistance, player.position.y, player.position.z},  // Left
            {player.position.x + wallCheckDistance, player.position.y, player.position.z}   // Right
        };

        tempPlayer.position = newPosition;
        for (int i = 0; i < 4; i++) {
            Vector3 checkPos = wallCheckPositions[i];
            ObjectEntity wallCheckPlayer = player;
            wallCheckPlayer.position = checkPos;
            for (int j = 0; j < blockCount; j++) {
                if (CCheckCollisionBoxes(wallCheckPlayer, blocks[j])) {
                    float playerBottom = player.position.y - player.size.y/2;
                    float playerTop = player.position.y + player.size.y/2;
                    float blockBottom = blocks[j].position.y - blocks[j].size.y/2;
                    float blockTop = blocks[j].position.y + blocks[j].size.y/2;
                    
                    if (playerBottom < blockTop && playerTop > blockBottom) {
                        // Adjust velocity based on direction
                        if (i == 0 && velocity.z > 0) velocity.z = 0;  // Forward
                        if (i == 1 && velocity.z < 0) velocity.z = 0;  // Back
                        if (i == 2 && velocity.x < 0) velocity.x = 0;  // Left
                        if (i == 3 && velocity.x > 0) velocity.x = 0;  // Right
                    }
                }
            }
        }

        // Update position with wall-checked velocity
        newPosition = Vector3Add(player.position, Vector3Scale(velocity, deltaTime));
        tempPlayer.position = newPosition;

        // Ground and fall check
        bool groundDetected = false;
        int groundIndex = -1;
        bool supportFound = false;
        float groundCheckDistance = 0.01f;

        // Check 8 directions around player plus directly below for support
        Vector3 supportCheckPositions[9] = {
            {player.position.x, player.position.y - groundCheckDistance, player.position.z},          // Below
            {player.position.x + groundCheckDistance, player.position.y - groundCheckDistance, player.position.z},  // Below-right
            {player.position.x - groundCheckDistance, player.position.y - groundCheckDistance, player.position.z},  // Below-left
            {player.position.x, player.position.y - groundCheckDistance, player.position.z + groundCheckDistance},  // Below-forward
            {player.position.x, player.position.y - groundCheckDistance, player.position.z - groundCheckDistance},  // Below-back
            {player.position.x + groundCheckDistance, player.position.y - groundCheckDistance, player.position.z + groundCheckDistance}, // Below-right-forward
            {player.position.x + groundCheckDistance, player.position.y - groundCheckDistance, player.position.z - groundCheckDistance}, // Below-right-back
            {player.position.x - groundCheckDistance, player.position.y - groundCheckDistance, player.position.z + groundCheckDistance}, // Below-left-forward
            {player.position.x - groundCheckDistance, player.position.y - groundCheckDistance, player.position.z - groundCheckDistance}  // Below-left-back
        };

        for (int i = 0; i < blockCount; i++) {
            if (CCheckCollisionBoxes(tempPlayer, blocks[i])) {
                Vector3 blockMax = Vector3Add(blocks[i].position, Vector3Scale(blocks[i].size, 0.5f));
                Vector3 playerMin = Vector3Subtract(tempPlayer.position, Vector3Scale(tempPlayer.size, 0.5f));
                
                if (playerVelocityY <= 0 && playerMin.y <= blockMax.y) {
                    newPosition.y = blockMax.y + player.size.y/2;
                    playerVelocityY = 0;
                    player.state = STATE_GROUNDED;
                    groundDetected = true;
                    groundIndex = i;
                    supportFound = true;
                }
                else if (playerVelocityY > 0 && playerMin.y < blockMax.y) {
                    newPosition.y = blocks[i].position.y - blocks[i].size.y/2 - player.size.y/2;
                    playerVelocityY = 0;
                    supportFound = true;
                }
            }
        }

        // Additional support check if no direct collision
        if (!supportFound) {
            for (int i = 0; i < 9; i++) {
                ObjectEntity supportCheckPlayer = player;
                supportCheckPlayer.position = supportCheckPositions[i];
                for (int j = 0; j < blockCount; j++) {
                    if (CCheckCollisionBoxes(supportCheckPlayer, blocks[j])) {
                        supportFound = true;
                        break;
                    }
                }
                if (supportFound) break;
            }
        }

        // If no support found, ensure falling state
        if (!supportFound && player.state == STATE_GROUNDED) {
            player.state = STATE_FALLING;
        }

        // Reset all block colors to GRAY, then set current ground block to ORANGE
        for (int i = 0; i < blockCount; i++) {
            if (blocks[i].color.r != RED.r && blocks[i].color.r != BLUE.r) {  // Exclude enemy and wall blocks
                blocks[i].color = GRAY;
            }
        }
        if (groundIndex != -1) {
            blocks[groundIndex].color = ORANGE;
        }

        // Final position update
        player.position = newPosition;

        // Fall out of world check
        if (player.position.y < -100.0f) {
            player.position = (Vector3){0.0f, 2.9f, 0.0f};
            playerVelocityY = 0.0f;
            player.state = STATE_GROUNDED;
            player.color = GREEN;
        }

        player.color = (player.state == STATE_GROUNDED) ? GREEN : YELLOW;

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawCubeV(player.position, player.size, player.color);
                DrawCubeWiresV(player.position, player.size, DARKGRAY);
                
                for (int i = 0; i < blockCount; i++) {
                    Vector3 drawSize = Vector3Scale(blocks[i].size, 0.98f);
                    DrawCubeV(blocks[i].position, drawSize, blocks[i].color);
                    DrawCubeWiresV(blocks[i].position, blocks[i].size, DARKGRAY);
                }
            EndMode3D();

            char positionText[64];
            sprintf(positionText, "Position: X: %.2f Y: %.2f Z: %.2f State: %d", 
                   player.position.x, player.position.y, player.position.z, player.state);
            DrawText(positionText, 10, 70, 20, GRAY);
            
            DrawText("Left click to capture mouse, ESC to release", 10, 10, 20, GRAY);
            DrawText("WASD to move, SPACE to jump, R to reset", 10, 30, 20, GRAY);
            DrawFPS(10, 50);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}