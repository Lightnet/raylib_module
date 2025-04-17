// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
// #include "raylib.h"
// #include "raymath.h" // For quaternion and matrix operations
// #include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
//#include "flecs.h"
#include "flecs_module.h"
#include "flecs_raylib.h"

void setup_world_scene(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid) return;
  ecs_print(1,"set up scene");

  // // Create floor entity
  ecs_entity_t floor = ecs_new(it->world);
  ecs_set_name(it->world, floor, "Floor");
  ecs_set(it->world, floor, Transform3D, {
    .position = (Vector3){0.0f, -1.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){20.0f, 0.5f, 20.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity()
  });
  Model floorModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f)); // Double extents
  ecs_set(it->world, floor, ModelComponent, {
    .model=floorModel,
    .isLoaded=true
  });

  // Create cube entity
  ecs_entity_t cube = ecs_new(it->world);
  ecs_set_name(it->world, cube, "Cube");
  ecs_set(it->world, cube, Transform3D, {
    .position = (Vector3){0.0f, 2.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){1.0f, 1.0f, 1.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity()
  });

  // Load cube model and store in ModelComponent
  Model cubeModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, cube, ModelComponent, {
    .model=cubeModel,
    .isLoaded=true
  });


  // child
  ecs_entity_t node2 = ecs_new(it->world);
  ecs_set_name(it->world, node2, "NodeChild");
  ecs_set(it->world, node2, Transform3D, {
      .position = (Vector3){2.0f, 0.0f, 0.0f},
      .rotation = QuaternionIdentity(),
      .scale = (Vector3){0.5f, 0.5f, 0.5f},
      .localMatrix = MatrixIdentity(),
      .worldMatrix = MatrixIdentity()
  });
  ecs_add_pair(it->world, node2, EcsChildOf, cube);

  Model cubeModel02 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, node2, ModelComponent, {
    //&cube
    .model=cubeModel02,
    .isLoaded=true
  });

  rl_ctx->isLoaded=true;
}

Transform3D setTransform3DPos(Transform3D transform, Vector3 position) {
  // Update the position
  transform.position = position;
  
  // Set localMatrix to translation matrix based on position
  transform.localMatrix = MatrixTranslate(position.x, position.y, position.z);
  
  // Optionally, update worldMatrix (assuming no parent transform)
  transform.worldMatrix = transform.localMatrix;
  
  // Return the updated transform
  return transform;
}

void user_input_system(ecs_iter_t *it){
  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);
  float dt = GetFrameTime();
  // static bool isMovementMode = true;
  // static bool tabPressed = false;

  //ecs_print(1,"dt %.8f", dt);
  bool isFound = false;
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "Cube") == 0) {
        //if (IsKeyPressed(KEY_TAB) && !pi_ctx->tabPressed) {
        if (IsKeyPressed(KEY_TAB)) {
          // pi_ctx->tabPressed = true;
          pi_ctx->isMovementMode = !pi_ctx->isMovementMode;
          printf("Toggled mode to: %s\n", pi_ctx->isMovementMode ? "Movement" : "Rotation");
        }

        if (pi_ctx->isMovementMode) {
          if (IsKeyDown(KEY_W)) t[i].position.z -= 2.0f * dt;
          if (IsKeyDown(KEY_S)) t[i].position.z += 2.0f * dt;
          if (IsKeyDown(KEY_A)) t[i].position.x -= 2.0f * dt;
          if (IsKeyDown(KEY_D)) t[i].position.x += 2.0f * dt;
        }else{
          float rotateSpeed = 90.0f;
          if (IsKeyDown(KEY_Q)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 1, 0}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
          if (IsKeyDown(KEY_E)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 1, 0}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
          if (IsKeyDown(KEY_W)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){1, 0, 0}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
          if (IsKeyDown(KEY_S)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){1, 0, 0}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
          if (IsKeyDown(KEY_A)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 0, 1}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
          if (IsKeyDown(KEY_D)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 0, 1}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
          }
        }

        if (IsKeyPressed(KEY_R)) {
          t[i].position = (Vector3){0.0f, 0.0f, 0.0f};
          t[i].rotation = QuaternionIdentity();
          t[i].scale = (Vector3){1.0f, 1.0f, 1.0f};
        }
        
        // t[i] = setTransform3DPos(t[i], t[i].position);
        isFound = true;
        break;      
      } else if (strcmp(name, "Floor") == 0) {
        // t[i] = setTransform3DPos(t[i], t[i].position);
      }
    }
  }
  if(isFound){
    
  }
}

void rl_hud_render2d_system(ecs_iter_t *it){
  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;
  Transform3D *t = ecs_field(it, Transform3D, 0);
  // ecs_print(1,"hud");
  if(it->count == 1){
    for (int i = 0; i < it->count; i++) {
      const char *name = ecs_get_name(it->world, it->entities[i]);
      DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 80+20*i, 20, DARKGRAY);
    }
  }

  if(it->count == 2){
    for (int i = 0; i < it->count; i++) {
      const char *name = ecs_get_name(it->world, it->entities[i]);
      DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 120+20*i, 20, DARKGRAY);
    }
  }

  if(it->count == 1){
    DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 10, 20, DARKGRAY);
  }

  if(it->count == 2){
    DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 10+30, 20, DARKGRAY);
  }

  DrawText(TextFormat("Toggled mode to: %s\n", pi_ctx->isMovementMode ? "Movement" : "Rotation"), 10, 200, 20, DARKGRAY);
}

int main() {
  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  bool isRunning = false;
  flecs_module_init(world);
  flecs_raylib_module_init(world);

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "setup_world_scene", 
        .add = ecs_ids(ecs_dependson(OnSetupWorldPhase)) 
    }),
    .callback = setup_world_scene
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "user_input_system", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      //{ .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = user_input_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_hud_render2d_system", .add = ecs_ids(ecs_dependson(Render2DPhase)) }),
    .query.terms = {
        { .id = ecs_id(Transform3D), .src.id = EcsSelf },
        { .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = rl_hud_render2d_system
  });


  while (!isRunning) {
    RayLibContext *rl_ctx = ecs_singleton_ensure(world, RayLibContext);
    if(!rl_ctx) return;
    float dt = GetFrameTime();
    // ecs_print(1,"dt %.8f", dt);
    ecs_progress(world, dt);
    isRunning = rl_ctx->shouldQuit;
  }


  printf("clean up\n");
  // clean up
  CloseWindow();
  // UnloadModel(cubeModel); // Unload the model
  // clean model
  // UnloadModel(floorModel);
  // UnloadModel(cubeModel);
  // clean up world
  ecs_fini(world);
  printf("exit application finish!\n");

}

