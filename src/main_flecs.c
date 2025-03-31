// normal test build for flecs
#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
#include "raylib.h"
#include "raymath.h" // For quaternion and matrix operations
#include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
#include "flecs.h"



// Transform3D component
typedef struct {
  Vector3 position;
  Quaternion rotation;
  Vector3 scale;
  Matrix localMatrix;
  Matrix worldMatrix;
} Transform3D;
ECS_COMPONENT_DECLARE(Transform3D);

// Pointer component for raylib Model
typedef struct {
  Model* model;
} ModelComponent;
ECS_COMPONENT_DECLARE(ModelComponent);

// WorldContext struct to hold both Camera3D and later add variable later.
typedef struct {
  //JoltcPhysics *physics;
  Camera3D *camera;
} WorldContext;

// Custom trace handler //raylib to hide console default setup
static void TraceImpl(const char* message) {
  printf("Trace: %s\n", message);
}

// Custom logging function
void CustomLog(int msgType, const char *text, va_list args)
{
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

// Logic update system
void LogicUpdateSystem(ecs_iter_t *it) {
  //...
  // printf("update\n");
}

// Render begin system
void RenderBeginSystem(ecs_iter_t *it) {
  // printf("RenderBeginSystem\n");
  BeginDrawing();
  ClearBackground(RAYWHITE);
  //ClearBackground(GRAY);
}

// Begin camera system
void BeginCameraSystem(ecs_iter_t *it) {
  // printf("BeginCameraSystem\n");
  WorldContext *ctx = (WorldContext *)ecs_get_ctx(it->world);
  if (!ctx || !ctx->camera) return;
  BeginMode3D(*(ctx->camera));
}

//update object mesh transform
void CameraRenderSystem(ecs_iter_t *it) {
  // printf("CameraRenderSystem\n");
  Transform3D *t = ecs_field(it, Transform3D, 0);
  ModelComponent *m = ecs_field(it, ModelComponent, 1);
  
  for (int i = 0; i < it->count; i++) {
      if (m[i].model) {
          // Get entity name
          const char *name = ecs_get_name(it->world, it->entities[i]);
          Color color = RED; // Default color
          
          if (name) {
              if (strcmp(name, "Cube") == 0) {
                  color = RED;
              } else if (strcmp(name, "Floor") == 0) {
                  color = GRAY;
              }
          }
          
          // Update model transform with world matrix from physics
          m[i].model->transform = t[i].worldMatrix;
          DrawModelWires(*m[i].model, (Vector3){0,0,0}, 1.0f, color);
      }
  }
  DrawGrid(10, 1.0f);
}

// End camera system
void EndCameraSystem(ecs_iter_t *it) {
  // printf("EndCameraSystem\n");
  WorldContext *ctx = (WorldContext *)ecs_get_ctx(it->world);
  if (!ctx || !ctx->camera) return;
  EndMode3D();
}

// Render system
//2D only can't use 3D
void RenderSystem(ecs_iter_t *it) {
  //...
  // printf("RenderSystem\n");
  DrawFPS(10, 10);
}

// Physics system
void PhysicsSystem(ecs_iter_t *it) {
  //printf("PhysicsSystem\n");
}

// Render end system
void RenderEndSystem(ecs_iter_t *it) {
  // printf("RenderEndSystem\n");
  EndDrawing();
}

int main() {
  // Set custom logger
  SetTraceLogCallback(CustomLog);

  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  // Declare components
  ECS_COMPONENT(world, Transform3D);
  ECS_COMPONENT(world, ModelComponent);
  //ECS_COMPONENT(world, PhysicsBody);

  // Initialize raylib
  InitWindow(800, 600, "Jolt + Raylib Test");
  SetTargetFPS(60);

  // Adjust camera to properly view the scene
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };  // Moved up and back to see floor and cube
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Looking at origin
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Flecs set up
  // Define custom phases
  ecs_entity_t LogicUpdatePhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t BeginRenderPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t BeginCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t UpdateCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t EndCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t RenderPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t EndRenderPhase = ecs_new_w_id(world, EcsPhase);

  // Set phase dependencies
  ecs_add_pair(world, LogicUpdatePhase, EcsDependsOn, EcsPreUpdate);
  ecs_add_pair(world, BeginRenderPhase, EcsDependsOn, LogicUpdatePhase);
  ecs_add_pair(world, BeginCameraPhase, EcsDependsOn, BeginRenderPhase);
  ecs_add_pair(world, UpdateCameraPhase, EcsDependsOn, BeginCameraPhase);
  ecs_add_pair(world, EndCameraPhase, EcsDependsOn, UpdateCameraPhase);
  ecs_add_pair(world, RenderPhase, EcsDependsOn, EndCameraPhase);
  ecs_add_pair(world, EndRenderPhase, EcsDependsOn, RenderPhase);

  // Initialize systems
  ecs_system_init(world, &(ecs_system_desc_t){
   .entity = ecs_entity(world, { .name = "LogicUpdateSystem", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
   //.query.terms = {
       //{ .id = ecs_id(Transform3D), .src.id = EcsSelf },
       //{ .id = ecs_id(PhysicsBody), .src.id = EcsSelf }
   //},
   .callback = LogicUpdateSystem
  });
  //physics update.
  ecs_system_init(world, &(ecs_system_desc_t){
     .entity = ecs_entity(world, { .name = "PhysicsSystem", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
    //  .query.terms = {
    //      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
    //      { .id = ecs_id(PhysicsBody), .src.id = EcsSelf }
    //  },
     .callback = PhysicsSystem
  });
  // render the screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "RenderBeginSystem", .add = ecs_ids(ecs_dependson(BeginRenderPhase)) }),
    .callback = RenderBeginSystem
  });

  //render started for camera 3d model only
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "BeginCameraSystem", .add = ecs_ids(ecs_dependson(BeginCameraPhase)) }),
    .callback = BeginCameraSystem
  });

  //this for mesh model only
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "CameraRenderSystem", .add = ecs_ids(ecs_dependson(UpdateCameraPhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      { .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    }, 
    .callback = CameraRenderSystem
  });

  //finish camera render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "EndCameraSystem", .add = ecs_ids(ecs_dependson(EndCameraPhase)) }),
    .callback = EndCameraSystem
  });
  //render 2d screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "RenderSystem", .add = ecs_ids(ecs_dependson(RenderPhase)) }),
    .callback = RenderSystem
  });
  //finish render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "RenderEndSystem", .add = ecs_ids(ecs_dependson(EndRenderPhase)) }),
    .callback = RenderEndSystem
  });

  // Combine into WorldContext
  WorldContext world_ctx = {
    //.physics = &jphysics, //joltc physics
    .camera = &camera //raylib camera
  };
  ecs_set_ctx(world, &world_ctx, NULL);

  // Create floor entity
  ecs_entity_t floor = ecs_new(world);
  ecs_set_name(world, floor, "Floor");
  ecs_set(world, floor, Transform3D, {
    .position = (Vector3){0.0f, -1.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){20.0f, 0.5f, 20.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity()
  });
  //ecs_set(world, floor, PhysicsBody, {floorId});
  // Generate mesh with full dimensions (200x1x200) since scale is applied in transform
  Model floorModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f)); // Double extents
  ecs_set(world, floor, ModelComponent, {&floorModel});

  // Create cube entity
  ecs_entity_t cube = ecs_new(world);
  ecs_set_name(world, cube, "Cube");
  ecs_set(world, cube, Transform3D, {
    .position = (Vector3){0.0f, 2.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){1.0f, 1.0f, 1.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity()
  });
  //ecs_set(world, cube, PhysicsBody, {cubeId});
  Model cubeModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(world, cube, ModelComponent, {&cubeModel});

  printf("Starting main loop\n");
  //float time = 0.0f;
  while (!WindowShouldClose()) {
    //this is Flecs
    ecs_progress(world, 0);

    // BeginDrawing();
    // ClearBackground(RAYWHITE);

    // BeginMode3D(camera);

    // DrawCubeV((Vector3){floorPos.x, floorPos.y, floorPos.z}, 
    //           (Vector3){floorHalfExtents.x * 2, floorHalfExtents.y * 2, floorHalfExtents.z * 2}, 
    //           GRAY);
    // DrawCube((Vector3){sphereCurrentPos.x, sphereCurrentPos.y, sphereCurrentPos.z}, 1.0f, 1.0f, 1.0f, BLUE);
    // // DrawSphere((Vector3){sphereCurrentPos.x, sphereCurrentPos.y, sphereCurrentPos.z}, 0.5f, BLUE);
    // DrawGrid(10, 1.0f);

    // EndMode3D();

    // Reset cube with 'R' key
    if (IsKeyPressed(KEY_R)) {
      
    }
    // DrawFPS(10, 10);
    // DrawText(TextFormat("Sphere Y: %.2f, Vel Y: %.2f", sphereCurrentPos.y, sphereVelocity.y), 10, 30, 20, BLACK);
    // DrawText(TextFormat("Time: %.2f s", time), 10, 50, 20, BLACK);

    //EndDrawing();

    // if ((int)(time * 60) % 60 == 0) {
    //     printf("Time: %.2f, Sphere Y: %.2f, Vel Y: %.2f\n", time, sphereCurrentPos.y, sphereVelocity.y);
    // }
  }
  printf("clean up\n");

  CloseWindow();

  // Cleanup
  UnloadModel(floorModel);
  UnloadModel(cubeModel);

  ecs_fini(world);

  printf("finish\n");

  return 0;
}











