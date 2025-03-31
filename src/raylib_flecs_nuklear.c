#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h" // For quaternion and matrix operations
#include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
#include "flecs.h"

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "raylib-nuklear.h"

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
  struct nk_context *ctx;
  Camera3D *camera;
} WorldContext;


// Create a custom event
// ecs_entity_t Clicked = ecs_new(world);
ecs_entity_t Clicked;
ecs_entity_t Widget;


// Logic update system
void LogicUpdateSystem(ecs_iter_t *it) {
  //...
  // printf("update\n");
}

// End camera system
void UpdateNuklearSystem(ecs_iter_t *it) {
  // printf("EndCameraSystem\n");
  WorldContext *ctx = (WorldContext *)ecs_get_ctx(it->world);
  if (!ctx || !ctx->ctx) return;
  // Update the Nuklear context, along with input
  UpdateNuklear(ctx->ctx);

  // if (nk_begin(ctx->ctx, "Nuklear", nk_rect(100, 100, 220, 220),
  //         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
  //     nk_layout_row_static(ctx->ctx, 50, 150, 1);
  //     if (nk_button_label(ctx->ctx, "Button")) {
  //         // Button was clicked!
  //     }
  // }
  // nk_end(ctx->ctx);
}

// End camera system
void GUINuklearSystem(ecs_iter_t *it) {
  // printf("EndCameraSystem\n");
  WorldContext *ctx = (WorldContext *)ecs_get_ctx(it->world);
  if (!ctx || !ctx->ctx) return;
  // Update the Nuklear context, along with input
  // UpdateNuklear(ctx->ctx);

  if (nk_begin(ctx->ctx, "Nuklear", nk_rect(100, 100, 220, 220),
          NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
      nk_layout_row_static(ctx->ctx, 50, 150, 1);
      if (nk_button_label(ctx->ctx, "Button")) {
          // Button was clicked!
          // Emit entity event. Note how no component ids are provided.
          ecs_emit(it->world, &(ecs_event_desc_t) {
            .event = Clicked,
            .entity = Widget
          });
      }
  }
  nk_end(ctx->ctx);
}

void RenderNuklearSystem(ecs_iter_t *it) {
  // printf("EndCameraSystem\n");
  WorldContext *ctx = (WorldContext *)ecs_get_ctx(it->world);

  if (!ctx || !ctx->ctx) return;

  // Render the Nuklear GUI
  DrawNuklear(ctx->ctx);
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

}

// Render end system
void RenderEndSystem(ecs_iter_t *it) {
  // printf("RenderEndSystem\n");
  EndDrawing();
}

void OnClick(ecs_iter_t *it) {
  printf("onclick...\n");
}

int main(){
  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  // Declare components
  ECS_COMPONENT(world, Transform3D);
  ECS_COMPONENT(world, ModelComponent);
  //ECS_COMPONENT(world, PhysicsBody);

  // Initialize raylib
  InitWindow(800, 600, "Raylib Flecs Nuklear Test ");
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

  ecs_entity_t LogicGUIPhase = ecs_new_w_id(world, EcsPhase);

  ecs_entity_t BeginRenderPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t BeginCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t UpdateCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t EndCameraPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t RenderPhase = ecs_new_w_id(world, EcsPhase);

  ecs_entity_t BeginGUIPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t RenderGUIPhase = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t EndGUIPhase = ecs_new_w_id(world, EcsPhase);

  ecs_entity_t EndRenderPhase = ecs_new_w_id(world, EcsPhase);

  // Set phase dependencies
  ecs_add_pair(world, LogicUpdatePhase, EcsDependsOn, EcsPreUpdate);

  ecs_add_pair(world, LogicGUIPhase, EcsDependsOn, LogicUpdatePhase);

  ecs_add_pair(world, BeginRenderPhase, EcsDependsOn, LogicGUIPhase);
  ecs_add_pair(world, BeginCameraPhase, EcsDependsOn, BeginRenderPhase);
  ecs_add_pair(world, UpdateCameraPhase, EcsDependsOn, BeginCameraPhase);
  ecs_add_pair(world, EndCameraPhase, EcsDependsOn, UpdateCameraPhase);
  ecs_add_pair(world, RenderPhase, EcsDependsOn, EndCameraPhase);

  ecs_add_pair(world, BeginGUIPhase, EcsDependsOn, RenderPhase);
  ecs_add_pair(world, RenderGUIPhase, EcsDependsOn, BeginGUIPhase);
  ecs_add_pair(world, EndGUIPhase, EcsDependsOn, RenderGUIPhase);
  ecs_add_pair(world, EndRenderPhase, EcsDependsOn, EndGUIPhase);




  // Initialize systems
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "UpdateNuklearSystem", 
      .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) 
    }),
    .callback = UpdateNuklearSystem
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "GUINuklearSystem", 
      .add = ecs_ids(ecs_dependson(LogicGUIPhase)) 
    }),
    .callback = GUINuklearSystem
  });

  // Initialize systems
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "RenderNuklearSystem", 
      .add = ecs_ids(ecs_dependson(EndGUIPhase)) 
    }),
    //.query.terms = {
        //{ .id = ecs_id(Transform3D), .src.id = EcsSelf },
        //{ .id = ecs_id(PhysicsBody), .src.id = EcsSelf }
    //},
    .callback = RenderNuklearSystem
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "LogicUpdateSystem", 
      .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) 
    }),
    //.query.terms = {
        //{ .id = ecs_id(Transform3D), .src.id = EcsSelf },
        //{ .id = ecs_id(PhysicsBody), .src.id = EcsSelf }
    //},
    .callback = LogicUpdateSystem
  });

  //physics update.
  ecs_system_init(world, &(ecs_system_desc_t){
      .entity = ecs_entity(world, { .name = "PhysicsSystem", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
      //.query.terms = {
          //{ .id = ecs_id(Transform3D), .src.id = EcsSelf },
          //{ .id = ecs_id(PhysicsBody), .src.id = EcsSelf }
      //},
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

  // Create the Nuklear Context
  int fontSize = 10;
  //struct nk_context *ctx = InitNuklear(fontSize);
  // Combine into WorldContext
  WorldContext world_ctx = {
    .ctx = InitNuklear(fontSize),
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


  //GUI test
  Clicked = ecs_new(world);

  // Create entity
  Widget = ecs_entity(world, { .name = "widget" });

  // Create an entity observer
  ecs_observer_init(world, &(ecs_observer_desc_t){
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = Widget }},
    .events = { Clicked },
    .callback = OnClick
  });




  printf("Starting main loop\n");
  while (!WindowShouldClose()) {
      //this is Flecs
      ecs_progress(world, 0);
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
