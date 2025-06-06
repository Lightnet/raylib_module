#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
// #include "raylib.h"
// #include "raymath.h" // For quaternion and matrix operations
// #include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
// #include "flecs.h"

#include "flecs_module.h"
#include "flecs_raylib.h"
#include "flecs_raygui.h"
#include "flecs_dk_console.h"

#include <windows.h>

Vector3 MatrixGetPosition(Matrix mat){
  return (Vector3){ mat.m12, mat.m13, mat.m14 };
}

// Custom logging function
// void CustomLog(int msgType, const char *text, va_list args){
//   char timeStr[64] = { 0 };
//   time_t now = time(NULL);
//   struct tm *tm_info = localtime(&now);
//   strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
//   printf("[%s] ", timeStr);
//   switch (msgType)
//   {
//       case LOG_INFO: printf("[INFO] : "); break;
//       case LOG_ERROR: printf("[ERROR]: "); break;
//       case LOG_WARNING: printf("[WARN] : "); break;
//       case LOG_DEBUG: printf("[DEBUG]: "); break;
//       default: break;
//   }
//   vprintf(text, args);
//   printf("\n");
// }

// Function to check if any common key is pressed [input]
bool IsAnyKeyPressed(void){

    // Check common keys (letters, numbers, space, etc.) [input]
    int keys[] = {
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
        KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
        KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX,
        KEY_SEVEN, KEY_EIGHT, KEY_NINE,
        KEY_SPACE, KEY_ENTER, KEY_TAB
    };
    int keyCount = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < keyCount; i++)
    {
        if (IsKeyPressed(keys[i])) return true;
    }
    return false;
}

// Function to check if any mouse button is pressed [input]
bool IsAnyMouseButtonPressed(void){

    // Check all defined mouse buttons [input]
    int buttons[] = {
        MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_SIDE, MOUSE_BUTTON_EXTRA, MOUSE_BUTTON_FORWARD, MOUSE_BUTTON_BACK
    };
    int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

    for (int i = 0; i < buttonCount; i++)
    {
        if (IsMouseButtonPressed(buttons[i])) return true;
    }
    return false;
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
    .name = "Camera3DNode",
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


  ecs_entity_t node3 = ecs_entity(it->world, {
    .name = "Block",
  });
  ecs_set(it->world, node3, Transform3D, {
      .position = (Vector3){0.0f, 0.0f, 5.0f},
      .rotation = QuaternionIdentity(),
      .scale = (Vector3){1.0f, 1.0f, 1.0f},
      .localMatrix = MatrixIdentity(),
      .worldMatrix = MatrixIdentity(),
      .isDirty = true
  });
  // ecs_add_pair(it->world, node2, EcsChildOf, cube);

  Model cubeModel04 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  ecs_set(it->world, node3, ModelComponent, {
    .model=cubeModel04,
    .isLoaded=true
  });

  ecs_set(it->world, node3, CubeComponent, {0});

  rl_ctx->isLoaded=true;
}

void user_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  DKConsoleContext *dkc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if(!dkc_ctx || !dkc_ctx->console || dkc_ctx->console->is_open==true) return;

  if(c_ctx->currentMode != F_CAMERA_PLAYER) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);
  // float dt = GetFrameTime(); it->delta_time;
  // float dt = it->delta_time;

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
  int player_idx;
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "PlayerNode") == 0) {// parent for moving the player node
        //ecs_print(1,"Cube");
        bool wasModified = false;
        isFound = true;
        player_idx = i;
        break;
      } 
    }
  }

  if(isFound){
    //const char *name = ecs_get_name(it->world, it->entities[player_idx]);
    //t[player_idx]
    bool wasModified = false;

    Vector3 forward = {
      sinf(pi_ctx->yaw),              // X: Left/right (negative sin for Raylib’s +X right)
      0,                              // Y: Grounded (as you set)
      cosf(pi_ctx->yaw)               // Z: Forward/backward (negative cos for -Z forward)
    };

    float dt = GetFrameTime();
    float moveTime = pi_ctx->moveSpeed * dt;

    forward = Vector3Normalize(forward); // Ensure unit length
    forward.y = 0;//ground for now.
    Vector3 right = Vector3CrossProduct(forward, rl_ctx->camera.up);
    Vector3 originPos = t[player_idx].position;

    if (IsKeyDown(KEY_W)){
      // ecs_print(1,"forward");
      t[player_idx].position = Vector3Add(t[player_idx].position, Vector3Scale(forward, moveTime));
      t[player_idx].isDirty = true;
    }
    if (IsKeyDown(KEY_S)) {
      t[player_idx].position = Vector3Subtract(t[player_idx].position, Vector3Scale(forward, moveTime));
      t[player_idx].isDirty = true;
    }
    if (IsKeyDown(KEY_A)) {
      t[player_idx].position = Vector3Subtract(t[player_idx].position, Vector3Scale(right, moveTime));
      t[player_idx].isDirty = true;
    }
    if (IsKeyDown(KEY_D)) {
      t[player_idx].position = Vector3Add(t[player_idx].position, Vector3Scale(right, moveTime));
      t[player_idx].isDirty = true;
    }
    if (IsKeyPressed(KEY_R)) {
      t[player_idx].position = (Vector3){0.0f, 0.0f, 0.0f};
      t[player_idx].rotation = QuaternionIdentity();
      t[player_idx].scale = (Vector3){1.0f, 1.0f, 1.0f};
      wasModified = true;
    }
    if (wasModified) {
      //update matrix 3d
      t[player_idx].isDirty = true;
      // printf("Marked %s as dirty\n", name);
    }

    ecs_query_t *q = ecs_query(it->world, {
      .terms = {
        { .id = ecs_id(Transform3D) },
        { .id = ecs_id(CubeComponent) },
      }
    });

    ecs_iter_t s_it = ecs_query_iter(it->world, q);

    Vector3 playerPosition = t[player_idx].position;
    Vector3 playerSize = {1, 1, 1};

    while (ecs_query_next(&s_it)) {
      //ecs_print(1,"blocks %d", s_it.count);
      Transform3D *t_3d = ecs_field(&s_it, Transform3D, 0);
      CubeComponent *s_com = ecs_field(&s_it, CubeComponent, 1);
      Vector3 b_pos = MatrixGetPosition(t_3d->worldMatrix);
      // ecs_print(1,"x:%0.2f, y:%0.2f, z:%0.2f", b_pos.x,b_pos.y,b_pos.z);

      Vector3 enemyBoxPos = b_pos;
      Vector3 enemyBoxSize = { 1.0f, 1.0f, 1.0f };

      BoundingBox box1 = {
        (Vector3){playerPosition.x - playerSize.x/2,
                  playerPosition.y - playerSize.y/2,
                  playerPosition.z - playerSize.z/2},
        (Vector3){playerPosition.x + playerSize.x/2,
                  playerPosition.y + playerSize.y/2,
                  playerPosition.z + playerSize.z/2}
      };
      BoundingBox box2 = {
        (Vector3){enemyBoxPos.x - enemyBoxSize.x/2,
                  enemyBoxPos.y - enemyBoxSize.y/2,
                  enemyBoxPos.z - enemyBoxSize.z/2},
        (Vector3){enemyBoxPos.x + enemyBoxSize.x/2,
                  enemyBoxPos.y + enemyBoxSize.y/2,
                  enemyBoxPos.z + enemyBoxSize.z/2}
      };
      if(CheckCollisionBoxes(box1, box2)){
        ecs_print(1,"COLLISION!");
        t[player_idx].position = originPos;
      }


      // bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2);

    }





  }
}

// input mouse capture screen
void user_capture_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  DKConsoleContext *dkc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if(!dkc_ctx || !dkc_ctx->console || dkc_ctx->console->is_open==true) return;

  // ecs_print(1,"delta_time ");
  // ecs_print(1,"delta %d", it->delta_time);
  // ecs_print(1,"delta_system_time %d", it->delta_system_time);

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

  // if(!pi_ctx->isCaptureMouse && ( (key > 0) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )) {
  if(!pi_ctx->isCaptureMouse && ( IsAnyKeyPressed() || IsAnyMouseButtonPressed() )) {
    HideCursor();
    DisableCursor();  // Locks mouse to window
    pi_ctx->isCaptureMouse = true;
  }

  if (pi_ctx->isCaptureMouse && IsKeyPressed(KEY_ESCAPE)){
    EnableCursor();  // Release mouse
    ShowCursor();
    pi_ctx->isCaptureMouse = false;
  }

  // if(c_ctx->currentMode != F_CAMERA_FREE) return;

  // if (pi_ctx->isCaptureMouse){

  //   Vector2 mouseDelta = GetMouseDelta();  // Mouse movement [input]
  //   pi_ctx->yaw -= mouseDelta.x * pi_ctx->mouseSensitivity;
  //   pi_ctx->pitch -= mouseDelta.y * pi_ctx->mouseSensitivity;
  //   pi_ctx->pitch = Clamp(pi_ctx->pitch, -PI/2.0f + 0.1f, PI/2.0f - 0.1f);  // Limit pitch [raymath]

  //   // Update camera target based on yaw and pitch [raymath]
  //   rl_ctx->camera.target = Vector3Add(rl_ctx->camera.position, (Vector3){
  //     cosf(pi_ctx->pitch) * sinf(pi_ctx->yaw),
  //     sinf(pi_ctx->pitch),
  //     cosf(pi_ctx->pitch) * cosf(pi_ctx->yaw)
  //   });
  // }

  // // Camera movement based on direction [input, raymath]
  // Vector3 forward = Vector3Normalize(Vector3Subtract(rl_ctx->camera.target, rl_ctx->camera.position));  // Forward vector [raymath]
  // Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, rl_ctx->camera.up));  // Right vector [raymath]
  // Vector3 up = rl_ctx->camera.up;  // Global up vector (0, 1, 0) [raymath]

  // //ecs_print(1,"delta_time ");
  // // ecs_print(1,"delta %d", it->delta_time);

  // float dt = GetFrameTime();
  // float moveTime = pi_ctx->moveSpeed * dt;

  // // Forward/Backward (W/S)
  // if (IsKeyDown(KEY_W))  // Move forward [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, moveTime));  // [raymath]
  // if (IsKeyDown(KEY_S))  // Move backward [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, -moveTime)); // [raymath]

  // // Left/Right (A/D)
  // if (IsKeyDown(KEY_A))  // Move left [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, -moveTime));  // [raymath]
  // if (IsKeyDown(KEY_D))  // Move right [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, moveTime));   // [raymath]

  // // Up/Down (Space/Shift)
  // if (IsKeyDown(KEY_SPACE))  // Move up [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, moveTime));      // [raymath]
  // if (IsKeyDown(KEY_LEFT_SHIFT))  // Move down [input]
  //   rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, -moveTime));     // [raymath]

  // // Update camera target after movement
  // rl_ctx->camera.target = Vector3Add(rl_ctx->camera.position, forward);

}

void camera_first_person_mode_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

  DKConsoleContext *dkc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if(!dkc_ctx || !dkc_ctx->console || dkc_ctx->console->is_open==true) return;

  if(c_ctx->currentMode != F_CAMERA_PLAYER) return;

  Transform3D *t = ecs_field(it, Transform3D, 0);

  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (strcmp(name, "Camera3DNode") == 0) {
      Vector3 vecPos =  MatrixGetPosition(t[i].worldMatrix);
      rl_ctx->camera.position = Vector3Lerp(rl_ctx->camera.position, vecPos, 0.55f);//smooth camera
    }

  }

}

//camera3d free mode
void camera_free_mode_input_system(ecs_iter_t *it){

  DKConsoleContext *dkc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if(!dkc_ctx || !dkc_ctx->console || dkc_ctx->console->is_open==true) return;

  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;

  PlayerInput_T *pi_ctx = ecs_singleton_ensure(it->world, PlayerInput_T);
  if (!pi_ctx) return;

  CameraContext_T *c_ctx = ecs_singleton_ensure(it->world, CameraContext_T);
  if(!c_ctx) return;

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

  //ecs_print(1,"delta_time ");
  // ecs_print(1,"delta %d", it->delta_time);

  float dt = GetFrameTime();
  float moveTime = pi_ctx->moveSpeed * dt;

  // Forward/Backward (W/S)
  if (IsKeyDown(KEY_W))  // Move forward [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, moveTime));  // [raymath]
  if (IsKeyDown(KEY_S))  // Move backward [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(forward, -moveTime)); // [raymath]

  // Left/Right (A/D)
  if (IsKeyDown(KEY_A))  // Move left [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, -moveTime));  // [raymath]
  if (IsKeyDown(KEY_D))  // Move right [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(right, moveTime));   // [raymath]

  // Up/Down (Space/Shift)
  if (IsKeyDown(KEY_SPACE))  // Move up [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, moveTime));      // [raymath]
  if (IsKeyDown(KEY_LEFT_SHIFT))  // Move down [input]
    rl_ctx->camera.position = Vector3Add(rl_ctx->camera.position, Vector3Scale(up, -moveTime));     // [raymath]

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
      DrawText(TextFormat("[Tab] Camera Mode: F_CAMERA_FREE"), 10, 50, 20, DARKGRAY);
      break;
    case F_CAMERA_PLAYER:
      // ecs_print(1,"F_CAMERA_PLAYER");
      DrawText(TextFormat("[Tab] Camera Mode: F_CAMERA_PLAYER"), 10, 50, 20, DARKGRAY);
      break;
    case F_CAMERA_DEBUG:
      // ecs_print(1,"F_CAMERA_DEBUG");
      DrawText(TextFormat("[Tab] Camera Mode: F_CAMERA_DEBUG"), 10, 50, 20, DARKGRAY);
      break;
  }


  // DrawText(TextFormat("Toggled mode to: %s\n", pi_ctx->isMovementMode ? "Movement" : "Rotation"), 10, 30, 20, DARKGRAY);
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
  flecs_dk_console_module_init(world);
  // set up entity
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "setup_world_scene", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetupWorldPhase)) 
    }),
    .callback = setup_world_scene
  });
  // input capture and release mouse
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "user_capture_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .callback = user_capture_input_system
  });
  // camera 3d freee mode
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "camera_free_mode_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
    },
    .callback = camera_free_mode_input_system
  });
  // camera3d first person mode
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "camera_first_person_mode_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
    },
    .callback = camera_first_person_mode_input_system
  });
  // player input keys
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "user_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
    },
    .callback = user_input_system
  });
  // draw 2d
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_hud_render2d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.Render2D1Phase)) }),
    .query.terms = {
        { ecs_id(Transform3D)  }//,
        // { .id = ecs_id(Transform3D), .src.id = EcsSelf }//,
        //{ .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = rl_hud_render2d_system
  });

  // Initialize time variables
  clock_t last_time = clock();
  float delta_time = 0.0f;
  const float target_frame_time = 1.0f / 60.0f; // Target 60 FPS (16.66ms)
  const float fixed_time_step = 1.0f / 60.0f; // Fixed step for consistent updates
  float accumulated_time = 0.0f;
  
  while (!isRunning) {
    
    RayLibContext *rl_ctx = ecs_singleton_ensure(world, RayLibContext);
    if(!rl_ctx) return;
    // float dt = GetFrameTime();
    // ecs_print(1,"dt %.8f", dt);
    // ecs_progress(world, dt);
    
    // Calculate delta_time in seconds
    clock_t current_time = clock();
    delta_time = (float)(current_time - last_time) / CLOCKS_PER_SEC;
    last_time = current_time;

    // Cap delta_time to avoid large jumps
    if (delta_time > 0.1f) delta_time = 0.1f;

    // Accumulate time for fixed time step
    accumulated_time += delta_time;

    // Process fixed time steps
    while (accumulated_time >= fixed_time_step) {
      ecs_progress(world, fixed_time_step); // Use fixed time step for consistency
      // ecs_print(1, "loop delta_time %.8f", fixed_time_step);
      accumulated_time -= fixed_time_step;
    }

    // Frame rate control: Sleep to approximate 60 FPS
    clock_t frame_end_time = clock();
    float frame_time = (float)(frame_end_time - current_time) / CLOCKS_PER_SEC;
    float sleep_time = target_frame_time - frame_time;
    if (sleep_time > 0) {
        Sleep((DWORD)(sleep_time * 1000.0f)); // Convert seconds to milliseconds
    }

    // close loop
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

