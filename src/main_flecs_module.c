#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
// #include "raylib.h"
// #include "raymath.h" // For quaternion and matrix operations
// #include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
//#include "flecs.h"
#include "flecs_module.h"
#include "flecs_raylib.h"
#include "flecs_raygui.h"

// Custom logging function
void CustomLog(int msgType, const char *text, va_list args){
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
    .worldMatrix = MatrixIdentity(),
    .isDirty = true
  });
  Model floorModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f)); // Double extents
  ecs_set(it->world, floor, ModelComponent, {
    .model=floorModel,
    .isLoaded=true
  });

  // Create cube entity
  // ecs_entity_t cube = ecs_new(it->world);
  // ecs_set_name(it->world, cube, "Cube");
  ecs_entity_t cube = ecs_entity(it->world, {
    .name = "Cube"
  });

  ecs_set(it->world, cube, Transform3D, {
    .position = (Vector3){0.0f, 2.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){1.0f, 1.0f, 1.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity(),
    .isDirty = true
  });

  // Load cube model and store in ModelComponent
  Model cubeModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, cube, ModelComponent, {
    .model=cubeModel,
    .isLoaded=true
  });


  // child
  // ecs_entity_t node2 = ecs_new(it->world);
  // ecs_set_name(it->world, node2, "NodeChild");
  ecs_entity_t node2 = ecs_entity(it->world, {
    .name = "NodeChild",
    .parent = cube
  });
  ecs_set(it->world, node2, Transform3D, {
      .position = (Vector3){2.0f, 0.0f, 0.0f},
      .rotation = QuaternionIdentity(),
      .scale = (Vector3){0.5f, 0.5f, 0.5f},
      .localMatrix = MatrixIdentity(),
      .worldMatrix = MatrixIdentity()
  });
  // ecs_add_pair(it->world, node2, EcsChildOf, cube);

  Model cubeModel02 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, node2, ModelComponent, {
    //&cube
    .model=cubeModel02,
    .isLoaded=true
  });

  rl_ctx->isLoaded=true;
}

void user_input_system(ecs_iter_t *it){
  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);
  // float dt = GetFrameTime(); it->delta_time;
  float dt = it->delta_time;

  //ecs_print(1,"dt %.8f", dt);
  bool isFound = false;
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "Cube") == 0) {
        //ecs_print(1,"Cube");
        bool wasModified = false;
        if (IsKeyPressed(KEY_TAB)) {
          pi_ctx->isMovementMode = !pi_ctx->isMovementMode;
          printf("Toggled mode to: %s\n", pi_ctx->isMovementMode ? "Movement" : "Rotation");
        }

        if (pi_ctx->isMovementMode) {
          if (IsKeyDown(KEY_W)) {
            ecs_print(1,"Key W");
            t[i].position.z -= 2.0f * dt;
            wasModified = true;
          }
          if (IsKeyDown(KEY_S)) {t[i].position.z += 2.0f * dt;wasModified = true;}
          if (IsKeyDown(KEY_A)) {t[i].position.x -= 2.0f * dt;wasModified = true;}
          if (IsKeyDown(KEY_D)) {t[i].position.x += 2.0f * dt;wasModified = true;}
        }else{
          float rotateSpeed = 90.0f;
          if (IsKeyDown(KEY_Q)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 1, 0}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
          if (IsKeyDown(KEY_E)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 1, 0}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
          if (IsKeyDown(KEY_W)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){1, 0, 0}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
          if (IsKeyDown(KEY_S)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){1, 0, 0}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
          if (IsKeyDown(KEY_A)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 0, 1}, DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
          if (IsKeyDown(KEY_D)) {
            Quaternion rot = QuaternionFromAxisAngle((Vector3){0, 0, 1}, -DEG2RAD * rotateSpeed * dt);
            t[i].rotation = QuaternionMultiply(t[i].rotation, rot);
            wasModified = true;
          }
        }

        if (IsKeyPressed(KEY_R)) {
          t[i].position = (Vector3){0.0f, 0.0f, 0.0f};
          t[i].rotation = QuaternionIdentity();
          t[i].scale = (Vector3){1.0f, 1.0f, 1.0f};
          wasModified = true;
        }
        if (wasModified) {
          t[i].isDirty = true;
          printf("Marked %s as dirty\n", name);
        }
        
        break;      
      } else if (strcmp(name, "Floor") == 0) {
        // t[i] = setTransform3DPos(t[i], t[i].position);
      }
    }
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
      DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 50+20*i, 20, DARKGRAY);
    }
    DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 10+20, 20, DARKGRAY);
  }

  if(it->count == 2){
    for (int i = 0; i < it->count; i++) {
      const char *name = ecs_get_name(it->world, it->entities[i]);
      DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 90+20*i, 20, DARKGRAY);
    }
    DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 50+20*1, 20, DARKGRAY);
  }

  DrawText(TextFormat("Toggled mode to: %s\n", pi_ctx->isMovementMode ? "Movement" : "Rotation"), 10, 10, 20, DARKGRAY);
}

int main() {
  // Set custom logger
  // SetTraceLogCallback(CustomLog);

  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  bool isRunning = false;
  flecs_module_init(world);
  flecs_raylib_module_init(world);
  flecs_raygui_module_init(world);

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "setup_world_scene", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetupWorldPhase)) 
    }),
    .callback = setup_world_scene
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "user_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      //{ .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = user_input_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_hud_render2d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.Render2D1Phase)) }),
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

