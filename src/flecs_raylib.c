// raylib module need for handle entities
// this set up and run time update
#include "flecs_module.h"
#include "flecs_raylib.h"

// Function to check if the model exists/loaded
bool is_model_valid(ModelComponent* component) {
  if (component == NULL || !component->isLoaded) {
      return false;
  }
  // Additional check: ensure model has valid mesh data
  return component->model.meshCount > 0 && component->model.meshes != NULL;
}

void rl_setup_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;
  ecs_print(1,"setup raylib window");

  InitWindow(rl_ctx->width, rl_ctx->height, "main flecs");
  SetTargetFPS(60);
}

void rl_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;
  ECS_RL_INPUT_T *input = ecs_singleton_ensure(it->world, ECS_RL_INPUT_T);
  if(!input)return;
  //rl_ctx->shouldQuit = WindowShouldClose();
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

// Update transform for all nodes
void UpdateTransformSystem(ecs_iter_t *it) {
  Transform3D *transforms = ecs_field(it, Transform3D, 0); // Field 0: Self Transform3D
  Transform3D *parent_transforms = ecs_field(it, Transform3D, 1); // Field 1: Parent Transform3D (optional)
  
  for (int i = 0; i < it->count; i++) {
      ecs_entity_t entity = it->entities[i];
      Transform3D *transform = &transforms[i];
      
      // Update local transform
      Matrix translation = MatrixTranslate(transform->position.x, transform->position.y, transform->position.z);
      Matrix rotation = QuaternionToMatrix(transform->rotation);
      Matrix scaling = MatrixScale(transform->scale.x, transform->scale.y, transform->scale.z);
      transform->localMatrix = MatrixMultiply(scaling, MatrixMultiply(rotation, translation));

      // Get entity name and position
      const char *entity_name = ecs_get_name(it->world, entity) ? ecs_get_name(it->world, entity) : "(unnamed)";
      Vector3 entity_pos = transform->position;
      
      // Print child position
      // printf("child %s position (%.2f, %.2f, %.2f)\n", 
      //     entity_name, entity_pos.x, entity_pos.y, entity_pos.z);
      
      // Check if parent data is available
      if (ecs_field_is_set(it, 1)) { // Parent term is set
          Transform3D *parent_transform = &parent_transforms[i];
          ecs_entity_t parent = ecs_get_parent(it->world, entity);
          const char *parent_name = ecs_get_name(it->world, parent) ? ecs_get_name(it->world, parent) : "(unnamed)";
          Vector3 parent_pos = parent_transform->position;
          transform->worldMatrix = MatrixMultiply(transform->localMatrix, parent_transform->worldMatrix);
          // printf("-parent %s: position (%.2f, %.2f, %.2f), child world pos (%.2f, %.2f, %.2f)\n", 
          //     parent_name, parent_pos.x, parent_pos.y, parent_pos.z,
          //     transform->worldMatrix.m12, transform->worldMatrix.m13, transform->worldMatrix.m14);
      } else {
          transform->worldMatrix = transform->localMatrix; // No parent, use local as world
          // printf("-parent: None\n");
      }
      // printf("\n");

      //ecs_modified_id(it->world, entity, ecs_id(Transform3D)); // Notify Flecs of update
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
        .add = ecs_ids(ecs_dependson(OnSetUpPhase)) 
    }),
    .callback = rl_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_input_system", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
    .callback = rl_input_system
  });

  // render the screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_render_begin_system", .add = ecs_ids(ecs_dependson(BeginRenderPhase)) }),
    .callback = rl_render_begin_system
   });

   //render started for camera 3d model only
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_begin_camera3d_system", .add = ecs_ids(ecs_dependson(BeginCamera3DPhase)) }),
    .callback = rl_begin_camera3d_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_camera3d_system", .add = ecs_ids(ecs_dependson(UpdateCamera3DPhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      { .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = rl_camera3d_system
  });

  //finish camera render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_end_camera3d_system", .add = ecs_ids(ecs_dependson(EndCamera3DPhase)) }),
    .callback = rl_end_camera3d_system
  });

  //render 2d screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_render2d_system", .add = ecs_ids(ecs_dependson(Render2DPhase)) }),
    .callback = rl_render2d_system
  });

  //finish render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "rl_end_render_system", .add = ecs_ids(ecs_dependson(EndRenderPhase)) }),
    .callback = rl_end_render_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "UpdateTransformSystem", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
    .query.terms = {
        { .id = ecs_id(Transform3D), .src.id = EcsSelf },           // Term 0: Entity has Transform3D
        { .id = ecs_id(Transform3D), .src.id = EcsUp,              // Term 1: Parent has Transform3D
          .trav = EcsChildOf, .oper = EcsOptional }                // Traversal in ecs_term_t
    },
    .callback = UpdateTransformSystem
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
    .isLoaded = false
  });

  ecs_singleton_set(world, ECS_RL_INPUT_T, {0});

  ecs_singleton_set(world, PlayerInput_T, {
    .isMovementMode=true,
    .tabPressed=false
  });

}

