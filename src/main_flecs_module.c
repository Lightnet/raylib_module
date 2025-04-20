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

Vector3 MatrixGetPosition(Matrix mat)
{
    return (Vector3){ mat.m12, mat.m13, mat.m14 };
}

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

  // Create floor entity
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
  ecs_entity_t node01 = ecs_entity(it->world, {
    .name = "PlayerNode"
  });

  ecs_set(it->world, node01, Transform3D, {
    .position = (Vector3){0.0f, 1.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){1.0f, 1.0f, 1.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity(),
    .isDirty = true
  });

  // Load cube model and store in ModelComponent
  Model cubeModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, node01, ModelComponent, {
    .model=cubeModel,
    .isLoaded=true
  });


  // child
  // ecs_entity_t node2 = ecs_new(it->world);
  // ecs_set_name(it->world, node2, "NodeChild");
  ecs_entity_t node2 = ecs_entity(it->world, {
    .name = "NodeChild",
    // .parent = cube
  });
  ecs_set(it->world, node2, Transform3D, {
      .position = (Vector3){0.0f, 1.0f, 0.0f},
      .rotation = QuaternionIdentity(),
      .scale = (Vector3){0.5f, 0.5f, 0.5f},
      .localMatrix = MatrixIdentity(),
      .worldMatrix = MatrixIdentity()
  });
  ecs_add_pair(it->world, node2, EcsChildOf, node01);

  Model cubeModel02 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, node2, ModelComponent, {
    //&cube
    .model=cubeModel02,
    // .model=cubeModel,
    .isLoaded=true
  });



  // ecs_entity_t node3 = ecs_entity(it->world, {
  //   .name = "NodeChild3",
  //   //.parent = cube
  // });
  // ecs_set(it->world, node3, Transform3D, {
  //     .position = (Vector3){2.0f, 0.0f, 0.0f},
  //     .rotation = QuaternionIdentity(),
  //     .scale = (Vector3){0.5f, 0.5f, 0.5f},
  //     .localMatrix = MatrixIdentity(),
  //     .worldMatrix = MatrixIdentity()
  // });
  // // ecs_add_pair(it->world, node2, EcsChildOf, cube);

  // Model cubeModel03 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  // ecs_set(it->world, node3, ModelComponent, {
  //   //&cube
  //   .model=cubeModel03,
  //   .isLoaded=true
  // });

  rl_ctx->isLoaded=true;
}

void user_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  if(c_ctx->currentMode != F_CAMERA_PLAYER) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);
  // float dt = GetFrameTime(); it->delta_time;
  float dt = it->delta_time;

  Vector2 mouseDelta = GetMouseDelta();  // Mouse movement [input]
  pi_ctx->yaw -= mouseDelta.x * pi_ctx->mouseSensitivity;
  pi_ctx->pitch -= mouseDelta.y * pi_ctx->mouseSensitivity;
  pi_ctx->pitch = Clamp(pi_ctx->pitch, -PI/2.0f + 0.1f, PI/2.0f - 0.1f);  // Limit pitch [raymath]

  // Update camera target based on yaw and pitch [raymath]
  rl_ctx->camera.target = Vector3Add(rl_ctx->camera.position, (Vector3){
    cosf(pi_ctx->pitch) * sinf(pi_ctx->yaw),
    sinf(pi_ctx->pitch),
    cosf(pi_ctx->pitch) * cosf(pi_ctx->yaw)
  });

  //ecs_print(1,"dt %.8f", dt);
  bool isFound = false;
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "PlayerNode") == 0) {// parent for moving the player node
        //ecs_print(1,"Cube");
        bool wasModified = false;

        // Compute forward vector for camera direction
        // Vector3 forward = {
        //   cosf(pi_ctx->yaw) * cosf(pi_ctx->pitch),
        //   sinf(pi_ctx->pitch),
        //   sinf(pi_ctx->yaw) * cosf(pi_ctx->pitch)
        // };

        // Vector3 forward = {
        //   -sinf(pi_ctx->yaw),              // X: Left/right (negative sin for Raylib’s +X right)
        //   0,                       // Y: Grounded (as you set)
        //   -cosf(pi_ctx->yaw)               // Z: Forward/backward (negative cos for -Z forward)
        // };

        Vector3 forward = {
          sinf(pi_ctx->yaw),              // X: Left/right (negative sin for Raylib’s +X right)
          0,                       // Y: Grounded (as you set)
          cosf(pi_ctx->yaw)               // Z: Forward/backward (negative cos for -Z forward)
        };


        forward = Vector3Normalize(forward); // Ensure unit length
        forward.y = 0;//ground for now.
        Vector3 right = Vector3CrossProduct(forward, rl_ctx->camera.up);
        
        if (IsKeyDown(KEY_W)){
          ecs_print(1,"forward");
          t[i].position = Vector3Add(t[i].position, Vector3Scale(forward, pi_ctx->moveSpeed));
          t[i].isDirty = true;
        }
        if (IsKeyDown(KEY_S)) {
          t[i].position = Vector3Subtract(t[i].position, Vector3Scale(forward, pi_ctx->moveSpeed));
          t[i].isDirty = true;
        }
        if (IsKeyDown(KEY_A)) {
          t[i].position = Vector3Subtract(t[i].position, Vector3Scale(right, pi_ctx->moveSpeed));
          t[i].isDirty = true;
        }
        if (IsKeyDown(KEY_D)) {
          t[i].position = Vector3Add(t[i].position, Vector3Scale(right, pi_ctx->moveSpeed));
          t[i].isDirty = true;
        }

        if (IsKeyPressed(KEY_R)) {
          t[i].position = (Vector3){0.0f, 0.0f, 0.0f};
          t[i].rotation = QuaternionIdentity();
          t[i].scale = (Vector3){1.0f, 1.0f, 1.0f};
          wasModified = true;
        }
        if (wasModified) {
          //update matrix 3d
          t[i].isDirty = true;
          // printf("Marked %s as dirty\n", name);
        }

      } else if (strcmp(name, "NodeChild") == 0) {//camera {0,1,0}
        // t[i] = setTransform3DPos(t[i], t[i].position);
        //rl_ctx->camera
        Vector3 vecPos =  MatrixGetPosition(t[i].worldMatrix);
        rl_ctx->camera.position = (Vector3){ vecPos.x, vecPos.y, vecPos.z };
        //rl_ctx->camera.target = (Vector3){vecPos.x, vecPos.y, vecPos.z - 1.0f};
        //rl_ctx->camera.target = NULL;
      }else if (strcmp(name, "Floor") == 0) {
        // t[i] = setTransform3DPos(t[i], t[i].position);
      }
    }
  }
}

void user_capture_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  if (IsKeyPressed(KEY_TAB)){
    c_ctx->currentMode = (FCameraMode)((c_ctx->currentMode + 1) % 3); // Cycle through modes
    switch (c_ctx->currentMode){
      case F_CAMERA_FREE:
        ecs_print(1,"F_CAMERA_FREE");
        break;
      case F_CAMERA_PLAYER:
        ecs_print(1,"F_CAMERA_PLAYER");
        break;
      case F_CAMERA_DEBUG:
        ecs_print(1,"F_CAMERA_DEBUG");
        break;
    }
  }

  int key = GetKeyPressed();
  // ecs_print(1,"key press: %d", key);

  if(!pi_ctx->isCaptureMouse && ( (key > 0) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )) {
    HideCursor();
    DisableCursor();  // Locks mouse to window
    pi_ctx->isCaptureMouse = true;
  }

  if (pi_ctx->isCaptureMouse && IsKeyPressed(KEY_ESCAPE)){
    EnableCursor();  // Release mouse
    ShowCursor();
    pi_ctx->isCaptureMouse = false;
  }

  if(c_ctx->currentMode != F_CAMERA_FREE) return;

  if (pi_ctx->isCaptureMouse){

    Vector2 mouseDelta = GetMouseDelta();  // Mouse movement [input]
    pi_ctx->yaw -= mouseDelta.x * pi_ctx->mouseSensitivity;
    pi_ctx->pitch -= mouseDelta.y * pi_ctx->mouseSensitivity;
    pi_ctx->pitch = Clamp(pi_ctx->pitch, -PI/2.0f + 0.1f, PI/2.0f - 0.1f);  // Limit pitch [raymath]

    // Update camera target based on yaw and pitch [raymath]
    rl_ctx->camera.target = Vector3Add(rl_ctx->camera.position, (Vector3){
      cosf(pi_ctx->pitch) * sinf(pi_ctx->yaw),
      sinf(pi_ctx->pitch),
      cosf(pi_ctx->pitch) * cosf(pi_ctx->yaw)
    });
  }

  // Camera movement based on direction [input, raymath]
  Vector3 forward = Vector3Normalize(Vector3Subtract(rl_ctx->camera.target, rl_ctx->camera.position));  // Forward vector [raymath]
  Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, rl_ctx->camera.up));  // Right vector [raymath]
  Vector3 up = rl_ctx->camera.up;  // Global up vector (0, 1, 0) [raymath]

  // Forward/Backward (W/S)
  if (IsKeyDown(KEY_W))  // Move forward [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, pi_ctx->moveSpeed));  // [raymath]
  if (IsKeyDown(KEY_S))  // Move backward [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, -pi_ctx->moveSpeed)); // [raymath]

  // Left/Right (A/D)
  if (IsKeyDown(KEY_A))  // Move left [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, -pi_ctx->moveSpeed));  // [raymath]
  if (IsKeyDown(KEY_D))  // Move right [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, pi_ctx->moveSpeed));   // [raymath]

  // Up/Down (Space/Shift)
  if (IsKeyDown(KEY_SPACE))  // Move up [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, pi_ctx->moveSpeed));      // [raymath]
  if (IsKeyDown(KEY_LEFT_SHIFT))  // Move down [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, -pi_ctx->moveSpeed));     // [raymath]

  // Update camera target after movement
  rl_ctx->camera.target = Vector3Add(rl_ctx->camera.position, forward);

}

void rl_hud_render2d_system(ecs_iter_t *it){
  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);
  // ecs_print(1,"hud");

  // DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 10+20, 20, DARKGRAY);
  // for (int i = 0; i < it->count; i++) {
  //   const char *name = ecs_get_name(it->world, it->entities[i]);
  //   DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 50+20*i, 20, DARKGRAY);
  // }
  //DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 0, 20, DARKGRAY);
  // ecs_print(1,"Entities Rendered: %d", it->count);
  

  // if(it->count == 1){
  //   for (int i = 0; i < it->count; i++) {
  //     const char *name = ecs_get_name(it->world, it->entities[i]);
  //     DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 50+20*i, 20, DARKGRAY);
  //   }
  //   DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 10+20, 20, DARKGRAY);
  // }else {
  // // if(it->count == 2){
  //   for (int i = 0; i < it->count; i++) {
  //     const char *name = ecs_get_name(it->world, it->entities[i]);
  //     DrawText(TextFormat("Name: %s, idx: %d, x: %0.4f, y: %0.4f, z: %0.4f", name, i, t[i].position.x, t[i].position.y, t[i].position.z ), 10, 90+20*i, 20, DARKGRAY);
  //   }
  //   DrawText(TextFormat("Entities Rendered: %d", it->count), 10, 50+20*1, 20, DARKGRAY);
  // }

  switch (c_ctx->currentMode){
    case F_CAMERA_FREE:
      // ecs_print(1,"F_CAMERA_FREE");
      DrawText(TextFormat("Camera Mode: F_CAMERA_FREE"), 10, 30, 20, DARKGRAY);
      break;
    case F_CAMERA_PLAYER:
      // ecs_print(1,"F_CAMERA_PLAYER");
      DrawText(TextFormat("Camera Mode: F_CAMERA_PLAYER"), 10, 30, 20, DARKGRAY);
      break;
    case F_CAMERA_DEBUG:
      // ecs_print(1,"F_CAMERA_DEBUG");
      DrawText(TextFormat("Camera Mode: F_CAMERA_DEBUG"), 10, 30, 20, DARKGRAY);
      break;
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
    .entity = ecs_entity(world, { .name = "user_capture_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    // .query.terms = {
      // { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      //{ .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    // },
    .callback = user_capture_input_system
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
        { ecs_id(Transform3D)  }//,
        // { .id = ecs_id(Transform3D), .src.id = EcsSelf }//,
        //{ .id = ecs_id(ModelComponent), .src.id = EcsSelf }
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

