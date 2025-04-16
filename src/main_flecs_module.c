#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
#include "raylib.h"
#include "raymath.h" // For quaternion and matrix operations
#include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.
#include "flecs.h"

#include "flecs_module.h"
#include "flecs_raylib.h"


int main() {
  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  bool isRunning = false;
  flecs_module_init(world);
  flecs_raylib_module_init(world);

  while (!isRunning) {
    RayLibContext *rl_ctx = ecs_singleton_ensure(world, RayLibContext);
    if(!rl_ctx) return;
    ecs_progress(world, 0);
    isRunning = rl_ctx->shouldQuit;
  }

  printf("clean up\n");

  ecs_fini(world);

  printf("finish\n");

}


