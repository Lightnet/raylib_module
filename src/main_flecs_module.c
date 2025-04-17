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
  ecs_print(1,"set up scene");
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
  Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  Model cubeModel = LoadModelFromMesh(cubeMesh);
  ecs_set(it->world, cube, ModelComponent, {&cubeModel});
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

  while (!isRunning) {
    RayLibContext *rl_ctx = ecs_singleton_ensure(world, RayLibContext);
    if(!rl_ctx) return;
    ecs_progress(world, 0);
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
  printf("finish\n");

}


