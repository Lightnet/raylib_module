// raylib module need for handle entities
// this set up and run time update
#include "flecs_module.h"
#include "flecs_raylib.h"

// Helper function to update a single transform
void UpdateTransform(ecs_world_t *world, ecs_entity_t entity, Transform3D *transform) {
  // Get parent entity
  ecs_entity_t parent = ecs_get_parent(world, entity);
  const char *name = ecs_get_name(world, entity) ? ecs_get_name(world, entity) : "(unnamed)";
  bool parentIsDirty = false;

  // Check if parent is dirty
  if (parent && ecs_is_valid(world, parent)) {
      const Transform3D *parent_transform = ecs_get(world, parent, Transform3D);
      if (parent_transform && parent_transform->isDirty) {
          parentIsDirty = true;
      }
  }

  // Skip update if neither this transform nor its parent is dirty
  if (!transform->isDirty && !parentIsDirty) {
      // printf("Skipping update for %s (not dirty)\n", name);
      return;
  }

  // Compute local transform
  Matrix translation = MatrixTranslate(transform->position.x, transform->position.y, transform->position.z);
  Matrix rotation = QuaternionToMatrix(transform->rotation);
  Matrix scaling = MatrixScale(transform->scale.x, transform->scale.y, transform->scale.z);
  transform->localMatrix = MatrixMultiply(scaling, MatrixMultiply(rotation, translation));

  if (!parent || !ecs_is_valid(world, parent)) {
      // Root entity: world matrix = local matrix
      transform->worldMatrix = transform->localMatrix;
      printf("Root %s position (%.2f, %.2f, %.2f)\n", name, transform->position.x, transform->position.y, transform->position.z);
  } else {
      // Child entity: world matrix = local matrix * parent world matrix
      const Transform3D *parent_transform = ecs_get(world, parent, Transform3D);
      if (!parent_transform) {
          printf("Error: Parent %s lacks Transform3D for %s\n",
                 ecs_get_name(world, parent) ? ecs_get_name(world, parent) : "(unnamed)", name);
          transform->worldMatrix = transform->localMatrix;
          return;
      }

      // Validate parent world matrix
      float px = parent_transform->worldMatrix.m12;
      float py = parent_transform->worldMatrix.m13;
      float pz = parent_transform->worldMatrix.m14;
      if (fabs(px) > 1e6 || fabs(py) > 1e6 || fabs(pz) > 1e6) {
          printf("Error: Invalid parent %s world pos (%.2f, %.2f, %.2f) for %s\n",
                 ecs_get_name(world, parent) ? ecs_get_name(world, parent) : "(unnamed)",
                 px, py, pz, name);
          transform->worldMatrix = transform->localMatrix;
          return;
      }

      // Compute world matrix
      transform->worldMatrix = MatrixMultiply(transform->localMatrix, parent_transform->worldMatrix);

      // Extract world position
      float wx = transform->worldMatrix.m12;
      float wy = transform->worldMatrix.m13;
      float wz = transform->worldMatrix.m14;

      // Debug output
      const char *parent_name = ecs_get_name(world, parent) ? ecs_get_name(world, parent) : "(unnamed)";
      printf("Child %s (ID: %llu), parent %s (ID: %llu)\n",
             name, (unsigned long long)entity, parent_name, (unsigned long long)parent);
      printf("Child %s position (%.2f, %.2f, %.2f), parent %s world pos (%.2f, %.2f, %.2f), world pos (%.2f, %.2f, %.2f)\n",
             name, transform->position.x, transform->position.y, transform->position.z,
             parent_name, px, py, pz, wx, wy, wz);
  }

  // Mark children as dirty to ensure they update in the next frame
  ecs_iter_t it = ecs_children(world, entity);
  while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
          Transform3D *child_transform = ecs_get_mut(world, it.entities[i], Transform3D);
          if (child_transform) {
              child_transform->isDirty = true;
              //ecs_set(world, it.entities[i], Transform3D, *child_transform);
          }
      }
  }

  // Reset isDirty after updating
  transform->isDirty = false;
}

// Recursive function to process entity and its children
void ProcessEntityHierarchy(ecs_world_t *world, ecs_entity_t entity) {
  // Update the current entity's transform
  Transform3D *transform = ecs_get_mut(world, entity, Transform3D);
  bool wasUpdated = false;
  if (transform) {
      // Only update if dirty or parent is dirty
      ecs_entity_t parent = ecs_get_parent(world, entity);
      bool parentIsDirty = false;
      if (parent && ecs_is_valid(world, parent)) {
          const Transform3D *parent_transform = ecs_get(world, parent, Transform3D);
          if (parent_transform && parent_transform->isDirty) {
              parentIsDirty = true;
          }
      }
      if (transform->isDirty || parentIsDirty) {
          UpdateTransform(world, entity, transform);
          //ecs_set(world, entity, Transform3D, *transform); // Commit changes
          wasUpdated = true;
      }
  }

  // Skip processing children if this entity was not updated
  if (!wasUpdated) {
      return;
  }

  // Iterate through children
  ecs_iter_t it = ecs_children(world, entity);
  while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
          ecs_entity_t child = it.entities[i];
          ProcessEntityHierarchy(world, child); // Recursively process child
      }
  }
}

// System to update all transforms in hierarchical order
void UpdateTransformHierarchySystem(ecs_iter_t *it) {
  // Process only root entities (no parent)
  Transform3D *transforms = ecs_field(it, Transform3D, 0);
  for (int i = 0; i < it->count; i++) {
      ecs_entity_t entity = it->entities[i];
      ProcessEntityHierarchy(it->world, entity);
  }
}

// Function to check if the model exists/loaded
bool is_model_valid(ModelComponent* component) {
  if (component == NULL || !component->isLoaded) {
      return false;
  }
  // Additional check: ensure model has valid mesh data
  return component->model.meshCount > 0 && component->model.meshes != NULL;
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

void rl_setup_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;
  ecs_print(1,"setup raylib window");

  // Set custom logger
  // SetTraceLogCallback(CustomLog);

  InitWindow(rl_ctx->width, rl_ctx->height, "main flecs");
  SetExitKey(KEY_NULL);  // Set no key to close window automatically
  SetTargetFPS(60);
}

void rl_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;
  ECS_RL_INPUT_T *input = ecs_singleton_ensure(it->world, ECS_RL_INPUT_T);
  if(!input)return;
  //rl_ctx->shouldQuit = WindowShouldClose();
  // rl_ctx->shouldQuit = IsWindowCloseRequested();//nope
  // IsWindowState(FLAG_WINDOW_HIDPI)

  if(WindowShouldClose() == true && rl_ctx->isShutDown == false){
    rl_ctx->isShutDown = true;
    ecs_print(1,"RAYLIB WINDOW CLOSE!");
    ecs_emit(it->world, &(ecs_event_desc_t) {
      .event = ShutDownEvent,
      .entity = ShutDownModule
    });
  }

  int key = GetKeyPressed();
  // ecs_print(1,"PRESS %d", key);
  if(IsKeyPressed(key)){
    // ecs_print(1,"PRESS %d", key);
    input->keys[key].pressed = true;
    input->keys[key].state = true;
    input->keys[key].current = true;
  }
  // get key release different from raylib.
  for (int i = 0; i < 339; i++) {
    if(IsKeyReleased(i)){
      // ecs_print(1,"RELEASE %d", i);
      input->keys[i].pressed = false;
      input->keys[i].state = false;
      input->keys[i].current = false;
    }
  }

}

// Render begin system
void rl_render_begin_system(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;
  // printf("rl_render_begin_system\n");
  BeginDrawing();
  ClearBackground(RAYWHITE);
  //ClearBackground(GRAY);
}

// Begin camera system
void rl_begin_camera3d_system(ecs_iter_t *it) {
  //printf("rl_begin_camera3d_system\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || rl_ctx->isShutDown == true) return;
  BeginMode3D(rl_ctx->camera);
}

//update object mesh transform
void rl_camera3d_system(ecs_iter_t *it) {
  //printf("rl_camera3d_system\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || !rl_ctx->isLoaded || rl_ctx->isShutDown == true) return;

  PHComponent *ph_ctx = ecs_singleton_ensure(it->world, PHComponent);
  if (!ph_ctx) return;
  
  Transform3D *t = ecs_field(it, Transform3D, 0);
  ModelComponent *m = ecs_field(it, ModelComponent, 1);
  //ecs_print(1,"count %d", it->count);
  for (int i = 0; i < it->count; i++) {
    // if (m[i].isLoaded) {
    // }else{
    //   ecs_print(1,"null");
    // }
      if (is_model_valid(&m[i])) {
          // Get entity name
          const char *name = ecs_get_name(it->world, it->entities[i]);
          Color color = RED; // Default color
          
          if (name) {
              if (strcmp(name, "Cube") == 0) {
                  color = RED;
              } else if (strcmp(name, "Floor") == 0) {
                  color = GRAY;
              } else {
                color = BLUE;
              }
          }
          
          // Update model transform with world matrix from physics
          m[i].model.transform = t[i].worldMatrix;
          DrawModelWires(m[i].model, (Vector3){0,0,0}, 1.0f, color);
      }
  }
  DrawGrid(10, 1.0f);
}

// End camera system
void rl_end_camera3d_system(ecs_iter_t *it) {
  //printf("EndCamera3DSystem\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || rl_ctx->isShutDown == true) return;
  EndMode3D();
}

// Render system, 2D only can't use 3D
void rl_render2d_system(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isShutDown) return;
  //...
  // printf("Render2DSystem\n");
  DrawFPS(10, 10);
}

// Render end system
void rl_end_render_system(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || rl_ctx->isShutDown == true) return;
  // printf("rl_end_render_system\n");
  EndDrawing();
}

void rl_cleanup_system(ecs_world_t *world){

  ecs_print(1, "MODEL CLEAN UP...");

  ecs_query_t *q = ecs_query(world, {
    .terms = {
      { .id = ecs_id(ModelComponent) },
    }
  });

  ecs_iter_t s_it = ecs_query_iter(world, q);

  while (ecs_query_next(&s_it)) {
    ModelComponent *p = ecs_field(&s_it, ModelComponent, 0);
    for (int i = 0; i < s_it.count; i ++) {

      if(p[i].isLoaded){
        UnloadModel(p[i].model);
      }
    }
  }

}

void rl_cleanup_event_system(ecs_iter_t *it){

  rl_cleanup_system(it->world);

}

void rl_close_event_system(ecs_iter_t *it){
  ecs_print(1,"[module raylib] close_event_system");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;
  //rl_ctx->isShutDown = true;
  rl_ctx->shouldQuit = true;
  // close
  // CloseWindow(); // does not work
}

void rl_register_components(ecs_world_t *world){

  ECS_COMPONENT_DEFINE(world, ECS_RL_INPUT_T);
  ECS_COMPONENT_DEFINE(world, Transform3D);
  ECS_COMPONENT_DEFINE(world, ModelComponent);
  ECS_COMPONENT_DEFINE(world, RayLibContext);
  ECS_COMPONENT_DEFINE(world, PHComponent);
  ECS_COMPONENT_DEFINE(world, PlayerInput_T);

}

void rl_register_systems(ecs_world_t *world){

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CloseModule }},
    .events = { CloseEvent },
    .callback = rl_cleanup_event_system
  });
  
  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CloseModule }},
    .events = { CloseEvent },
    .callback = rl_close_event_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "rl_setup_system", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetUpPhase)) 
    }),
    .callback = rl_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_input_system", .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) }),
    .callback = rl_input_system
  });

  // render the screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_render_begin_system", .add = ecs_ids(ecs_dependson(GlobalPhases.BeginRenderPhase)) }),
    .callback = rl_render_begin_system
   });

   //render started for camera 3d model only
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_begin_camera3d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.BeginCamera3DPhase)) }),
    .callback = rl_begin_camera3d_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_camera3d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.UpdateCamera3DPhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      { .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = rl_camera3d_system
  });

  //finish camera render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_end_camera3d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.EndCamera3DPhase)) }),
    .callback = rl_end_camera3d_system
  });

  //render 2d screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_render2d_system", .add = ecs_ids(ecs_dependson(GlobalPhases.Render2D1Phase)) }),
    .callback = rl_render2d_system
  });

  //finish render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_end_render_system", .add = ecs_ids(ecs_dependson(GlobalPhases.EndRenderPhase)) }),
    .callback = rl_end_render_system
  });

  // LogicUpdatePhase
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, {
        .name = "UpdateTransformHierarchySystem",
        .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase))
    }),
    .query.terms = {
        { .id = ecs_id(Transform3D), .src.id = EcsSelf },
        { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot } // No parent
    },
    .callback = UpdateTransformHierarchySystem
  });

}

void flecs_raylib_module_init(ecs_world_t *world){
  ecs_print(1, "Initializing raylib module...");
  rl_register_components(world);
  rl_register_systems(world);

  // Adjust camera to properly view the scene
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };  // Moved up and back to see floor and cube
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Looking at origin
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  ecs_singleton_set(world, RayLibContext, {
    .width=800,
    .height=600,
    .shouldQuit=false,
    .hasError=false,
    .isShutDown=false,
    .camera = camera,
    .isCameraValid = true,
    .isLoaded = false,
    .isCaptureMouse = false,
  });

  ecs_singleton_set(world, ECS_RL_INPUT_T, {0});

  ecs_singleton_set(world, PlayerInput_T, {
    .isMovementMode=true,
    .tabPressed=false,
    .yaw=0.0f,
    .pitch=0.0f,
    .mouseSensitivity=0.005f,
    .moveSpeed=0.1f
  });

}

