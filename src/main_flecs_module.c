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

  // printf("  Mesh count: %d\n", cubeModel.meshCount);
  // printf("  Material count: %d\n", cubeModel.materialCount);
  // printf("  Vertex count (first mesh): %d\n", cubeModel.meshes[0].vertexCount);
  // Model cubeModel02 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  // ecs_singleton_set(it->world, PHComponent, {.model=&cubeModel02});
  // Model cubeModel03 = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  // g_model = cubeModel03;
  // printf("  Mesh count: %d\n", cubeModel03.meshCount);
  // printf("  Material count: %d\n", cubeModel03.materialCount);
  // printf("  Vertex count (first mesh): %d\n", cubeModel03.meshes[0].vertexCount);

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
  Transform3D *t = ecs_field(it, Transform3D, 0);
  float dt = GetFrameTime();
  //ecs_print(1,"dt %.8f", dt);
  bool isFound = false;
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "Cube") == 0) {
        if (IsKeyDown(KEY_W)){ 
          ecs_print(1,"Move");
          t[i].position.z -= 2.0f * dt;
        };
        if (IsKeyDown(KEY_S)) t[i].position.z += 2.0f * dt;
        if (IsKeyDown(KEY_A)) t[i].position.x -= 2.0f * dt;
        if (IsKeyDown(KEY_D)) t[i].position.x += 2.0f * dt;
        t[i] = setTransform3DPos(t[i], t[i].position);
        isFound = true;
        break;      
      } else if (strcmp(name, "Floor") == 0) {
        t[i] = setTransform3DPos(t[i], t[i].position);
      }
    }
  }
  if(isFound){
    
  }

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


