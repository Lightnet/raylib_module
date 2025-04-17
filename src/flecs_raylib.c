// 
#include "flecs_module.h"
#include "flecs_raylib.h"

// Function to check if the model exists/loaded
bool IsModelValid(ModelComponent* component) {
  if (component == NULL || !component->isLoaded) {
      return false;
  }
  // Additional check: ensure model has valid mesh data
  return component->model.meshCount > 0 && component->model.meshes != NULL;
}

// // Simulate loading a model
// myModel.model = LoadModel("path/to/model.obj");
// myModel.isLoaded = true; // Mark as loaded
// // Check if model is valid
// if (IsModelValid(&myModel)) {
//     DrawModel(myModel.model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
// } else {
//     DrawText("Model is not valid", 10, 10, 20, RED);
// }

void raylib_setup_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;
  ecs_print(1,"setup raylib window");

  InitWindow(rl_ctx->width, rl_ctx->height, "main flecs");
  SetTargetFPS(60);
}

void raylib_input_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;
  //rl_ctx->shouldQuit = WindowShouldClose();
  if(WindowShouldClose() == true && rl_ctx->isShutDown == false){
    rl_ctx->isShutDown = true;
    ecs_print(1,"RAYLIB WINDOW CLOSE!");
    ecs_emit(it->world, &(ecs_event_desc_t) {
      .event = ShutDownEvent,
      .entity = ShutDownModule
    });
  }
}

void raylib_move_system(ecs_iter_t *it){
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;

  for (int i = 0; i < it->count; i++) {
    //const char *name = ecs_get_name(it->world, it->entities[i]);

  }

}

// Logic update system
void LogicUpdateSystem(ecs_iter_t *it) {
  //...
  // printf("update\n");
}

// Render begin system
void RenderBeginSystem(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx || rl_ctx->isShutDown == true) return;
  // printf("RenderBeginSystem\n");
  BeginDrawing();
  ClearBackground(RAYWHITE);
  //ClearBackground(GRAY);
}

// Begin camera system
void BeginCamera3DSystem(ecs_iter_t *it) {
  //printf("BeginCamera3DSystem\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || rl_ctx->isShutDown == true) return;
  BeginMode3D(rl_ctx->camera);
}

//update object mesh transform
void CameraRender3DSystem(ecs_iter_t *it) {
  //printf("CameraRender3DSystem\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || !rl_ctx->isLoaded || rl_ctx->isShutDown == true) return;

  PHComponent *ph_ctx = ecs_singleton_ensure(it->world, PHComponent);
  if (!ph_ctx) return;


  // ecs_print(1," Mesh count: %d", ph_ctx->model->meshCount);
  // if(g_model){
    // ecs_print(1," Mesh count: %d", g_model.meshCount);
  // }
  

  Transform3D *t = ecs_field(it, Transform3D, 0);
  ModelComponent *m = ecs_field(it, ModelComponent, 1);
  //ecs_print(1,"count %d", it->count);
  for (int i = 0; i < it->count; i++) {
    // if (m[i].isLoaded) {
    // }else{
    //   ecs_print(1,"null");
    // }
      if (IsModelValid(&m[i].model)) {
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
          m[i].model.transform = t[i].worldMatrix;
          DrawModelWires(m[i].model, (Vector3){0,0,0}, 1.0f, color);
      }
  }
  DrawGrid(10, 1.0f);
}

// End camera system
void EndCamera3DSystem(ecs_iter_t *it) {
  //printf("EndCamera3DSystem\n");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isCameraValid || rl_ctx->isShutDown == true) return;
  EndMode3D();
}

// Render system, 2D only can't use 3D
void Render2DSystem(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || !rl_ctx->isShutDown) return;
  //...
  // printf("Render2DSystem\n");
  DrawFPS(10, 10);
}

// Render end system
void RenderEndSystem(ecs_iter_t *it) {
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if (!rl_ctx || rl_ctx->isShutDown == true) return;
  // printf("RenderEndSystem\n");
  EndDrawing();
}

void raylib_cleanup_system(ecs_world_t *world){

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

void raylib_cleanup_event_system(ecs_iter_t *it){

  raylib_cleanup_system(it->world);

}

void raylib_close_event_system(ecs_iter_t *it){
  ecs_print(1,"[module raylib] close_event_system");
  RayLibContext *rl_ctx = ecs_singleton_ensure(it->world, RayLibContext);
  if(!rl_ctx) return;
  //rl_ctx->isShutDown = true;
  rl_ctx->shouldQuit = true;
  // close
  //CloseWindow();
}

void raylib_register_components(ecs_world_t *world){

  ECS_COMPONENT_DEFINE(world, Transform3D);
  ECS_COMPONENT_DEFINE(world, ModelComponent);
  ECS_COMPONENT_DEFINE(world, RayLibContext);
  ECS_COMPONENT_DEFINE(world, PHComponent);

}

void raylib_register_systems(ecs_world_t *world){

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CloseModule }},
    .events = { CloseEvent },
    .callback = raylib_cleanup_event_system
  });
  
  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CloseModule }},
    .events = { CloseEvent },
    .callback = raylib_close_event_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "raylib_setup_system", 
        .add = ecs_ids(ecs_dependson(OnSetUpPhase)) 
    }),
    .callback = raylib_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "LogicUpdateSystem", .add = ecs_ids(ecs_dependson(LogicUpdatePhase)) }),
    .callback = raylib_input_system
  });

  // render the screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "RenderBeginSystem", .add = ecs_ids(ecs_dependson(BeginRenderPhase)) }),
    .callback = RenderBeginSystem
   });

   //render started for camera 3d model only
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "BeginCamera3DSystem", .add = ecs_ids(ecs_dependson(BeginCamera3DPhase)) }),
    .callback = BeginCamera3DSystem
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "CameraRender3DSystem", .add = ecs_ids(ecs_dependson(UpdateCamera3DPhase)) }),
    .query.terms = {
      { .id = ecs_id(Transform3D), .src.id = EcsSelf },
      { .id = ecs_id(ModelComponent), .src.id = EcsSelf }
    },
    .callback = CameraRender3DSystem
  });

  //finish camera render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "EndCamera3DSystem", .add = ecs_ids(ecs_dependson(EndCamera3DPhase)) }),
    .callback = EndCamera3DSystem
  });

  //render 2d screen
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "Render2DSystem", .add = ecs_ids(ecs_dependson(Render2DPhase)) }),
    .callback = Render2DSystem
  });

  //finish render
  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { .name = "RenderEndSystem", .add = ecs_ids(ecs_dependson(EndRenderPhase)) }),
    .callback = RenderEndSystem
  });

}

void flecs_raylib_module_init(ecs_world_t *world){
  ecs_print(1, "Initializing raylib module...");
  raylib_register_components(world);
  raylib_register_systems(world);

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

}

