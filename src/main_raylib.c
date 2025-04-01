#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#define ENET_IMPLEMENTATION
#include <enet.h>

typedef struct TransformNode {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    Matrix localMatrix;
    Matrix worldMatrix;
    struct TransformNode* parent;
    struct TransformNode** children;
    int childCount;
    bool isDirty;
    Model* model;
    Color color;
} TransformNode;

typedef struct Projectile {
    Vector3 position;
    Vector3 velocity;
    Vector3 startPosition;
    bool active;
} Projectile;

TransformNode CreateTransformNode(Vector3 pos, Quaternion rot, Vector3 scl, TransformNode* parent, Color color) {
    TransformNode node = { 
        .position = pos, 
        .rotation = rot, 
        .scale = scl, 
        .localMatrix = MatrixIdentity(), 
        .worldMatrix = MatrixIdentity(), 
        .parent = parent, 
        .children = NULL, 
        .childCount = 0, 
        .isDirty = true, 
        .model = NULL, 
        .color = color 
    };
    return node;
}

void AddChild(TransformNode* parent, TransformNode* child) {
    child->parent = parent;
    parent->childCount++;
    parent->children = realloc(parent->children, parent->childCount * sizeof(TransformNode*));
    parent->children[parent->childCount - 1] = child;
}

void MarkDirty(TransformNode* node) {
    node->isDirty = true;
    if (node->parent != NULL) MarkDirty(node->parent);
}

void UpdateTransform(TransformNode* node) {
    if (!node->isDirty && (node->parent != NULL && !node->parent->isDirty)) return;
    node->localMatrix = MatrixMultiply(MatrixScale(node->scale.x, node->scale.y, node->scale.z),
                                       MatrixMultiply(QuaternionToMatrix(node->rotation),
                                                      MatrixTranslate(node->position.x, node->position.y, node->position.z)));
    node->worldMatrix = (node->parent != NULL) ? MatrixMultiply(node->localMatrix, node->parent->worldMatrix) : node->localMatrix;
    node->isDirty = false;
    for (int i = 0; i < node->childCount; i++) {
        node->children[i]->isDirty = true;
        UpdateTransform(node->children[i]);
    }
}

void RenderNode(TransformNode* node) {
    if (node->model != NULL) {
        node->model->transform = node->worldMatrix;
        DrawModel(*node->model, (Vector3){0, 0, 0}, 1.0f, node->color);
    }
    for (int i = 0; i < node->childCount; i++) RenderNode(node->children[i]);
}

int main(void) {
    InitWindow(800, 600, "FPS with Fixed Target Positions");
    SetTargetFPS(60);

    Camera3D camera = { 
        .position = {0, 0, 0}, 
        .target = {0, 0, 1}, 
        .up = {0, 1, 0}, 
        .fovy = 90.0f, 
        .projection = CAMERA_PERSPECTIVE 
    };
    Model cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    TransformNode player = CreateTransformNode((Vector3){0, 0, 0}, QuaternionIdentity(), (Vector3){1, 1, 1}, NULL, WHITE);
    TransformNode gun = CreateTransformNode((Vector3){-0.4f, -0.3f, 0.5f}, QuaternionIdentity(), (Vector3){0.2f, 0.2f, 0.2f}, &player, RED);
    gun.model = &cube;
    AddChild(&player, &gun);

    #define TARGET_COUNT 3
    TransformNode targets[TARGET_COUNT];
    for (int i = 0; i < TARGET_COUNT; i++) {
        targets[i] = CreateTransformNode((Vector3){(float)(i - 1) * 5.0f, 0, 10.0f}, QuaternionIdentity(), (Vector3){0.5f, 0.5f, 0.5f}, NULL, BLUE);
        targets[i].model = &cube;
        TraceLog(LOG_INFO, "Target %d position: (%.1f, %.1f, %.1f)", i, targets[i].position.x, targets[i].position.y, targets[i].position.z);
    }

    #define MAX_PROJECTILES 100
    Projectile projectiles[MAX_PROJECTILES] = {0};
    float projectileSpeed = 20.0f;
    float shootCooldown = 0.2f, lastShotTime = -shootCooldown;
    int score = 0;

    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.005f;
    float yaw = 0.0f, pitch = 0.0f;
    const float pitchLimit = PI / 2.0f - 0.01f;

    DisableCursor();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        if (IsCursorHidden()) {
            Vector2 mouseDelta = GetMouseDelta();
            yaw -= mouseDelta.x * mouseSensitivity;
            pitch = Clamp(pitch + mouseDelta.y * mouseSensitivity, -pitchLimit, pitchLimit);
            Quaternion yawQuat = QuaternionFromAxisAngle((Vector3){0, 1, 0}, yaw);
            Quaternion pitchQuat = QuaternionFromAxisAngle((Vector3){1, 0, 0}, pitch);
            player.rotation = QuaternionMultiply(yawQuat, pitchQuat);
            MarkDirty(&player);
        }

        Vector3 moveDir = {0};
        Vector3 forward = Vector3Transform((Vector3){0, 0, 1}, QuaternionToMatrix(player.rotation));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0, 1, 0}));
        if (IsKeyDown(KEY_W)) moveDir = Vector3Add(moveDir, forward);
        if (IsKeyDown(KEY_S)) moveDir = Vector3Subtract(moveDir, forward);
        if (IsKeyDown(KEY_A)) moveDir = Vector3Subtract(moveDir, right);
        if (IsKeyDown(KEY_D)) moveDir = Vector3Add(moveDir, right);
        if (Vector3Length(moveDir) > 0) {
            moveDir = Vector3Normalize(moveDir);
            player.position = Vector3Add(player.position, Vector3Scale(moveDir, moveSpeed * dt));
            MarkDirty(&player);
        }

        float currentTime = GetTime();
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && currentTime - lastShotTime >= shootCooldown) {
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!projectiles[i].active) {
                    UpdateTransform(&player);
                    projectiles[i].position = Vector3Transform(gun.position, gun.worldMatrix);
                    projectiles[i].startPosition = projectiles[i].position;
                    projectiles[i].velocity = Vector3Scale(forward, projectileSpeed);
                    projectiles[i].active = true;
                    lastShotTime = currentTime;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                for (int j = 0; j < TARGET_COUNT; j++) {
                    Vector3 targetWorldPos = targets[j].position;
                    float hitDistance = Vector3Distance(projectiles[i].position, targetWorldPos);
                    if (hitDistance < 1.0f) {
                        projectiles[i].active = false;
                        targets[j].position.z += 1000.0f;
                        score += 10;
                        MarkDirty(&targets[j]); // Mark target dirty after moving
                        break;
                    }
                }
                if (projectiles[i].active) {
                    projectiles[i].position = Vector3Add(projectiles[i].position, Vector3Scale(projectiles[i].velocity, dt));
                    float distance = Vector3Distance(projectiles[i].startPosition, projectiles[i].position);
                    if (distance > 1000.0f) {
                        projectiles[i].active = false;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_R)) {
            player.position = (Vector3){0, 0, 0};
            yaw = 0.0f; pitch = 0.0f;
            player.rotation = QuaternionIdentity();
            MarkDirty(&player);
            for (int i = 0; i < MAX_PROJECTILES; i++) projectiles[i].active = false;
            for (int i = 0; i < TARGET_COUNT; i++) {
                targets[i].position = (Vector3){(float)(i - 1) * 5.0f, 0, 10.0f};
                MarkDirty(&targets[i]); // Ensure reset positions are updated
            }
            score = 0;
        }

        // Update transforms for all top-level nodes
        UpdateTransform(&player);
        for (int i = 0; i < TARGET_COUNT; i++) {
            UpdateTransform(&targets[i]);
        }

        camera.position = player.position;
        Vector3 lookForward = Vector3Transform((Vector3){0, 0, 1}, QuaternionToMatrix(player.rotation));
        camera.target = Vector3Add(camera.position, lookForward);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        
        DrawPlane((Vector3){0, 0, 0}, (Vector2){20, 20}, GRAY);
        RenderNode(&player);
        for (int i = 0; i < TARGET_COUNT; i++) {
            RenderNode(&targets[i]);
            DrawSphere(targets[i].position, 1.0f, Fade(YELLOW, 0.3f));
        }
        
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                DrawCube(projectiles[i].position, 0.1f, 0.1f, 0.1f, GREEN);
                DrawSphere(projectiles[i].position, 1.0f, Fade(GREEN, 0.3f));
            }
        }

        DrawGrid(20, 1.0f);
        EndMode3D();

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        DrawLine(screenWidth/2 - 10, screenHeight/2, screenWidth/2 + 10, screenHeight/2, BLACK);
        DrawLine(screenWidth/2, screenHeight/2 - 10, screenWidth/2, screenHeight/2 + 10, BLACK);

        DrawText("WASD: Move | Mouse: Look | LMB: Shoot | R: Reset | ESC: Toggle Mouse", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Score: %d", score), 10, 50, 20, DARKGRAY);
        DrawFPS(10, 30);
        EndDrawing();
    }

    UnloadModel(cube);
    if (player.children != NULL) {
        free(player.children);
    }
    CloseWindow();
    return 0;
}