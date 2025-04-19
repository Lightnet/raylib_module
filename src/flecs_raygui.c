

#include "flecs_raygui.h"
// #include "flecs_module.h"
// #include <raylib.h>
// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"
// #undef RAYGUI_IMPLEMENTATION


void render3d_raygui_system(ecs_iter_t *it){
  if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")){};
}

void rg_register_components(ecs_world_t *world){
  // 
}

void rg_register_systems(ecs_world_t *world){

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "render3d_raygui_system", 
      .add = ecs_ids(ecs_dependson(GlobalPhases.Render2D2Phase)) 
    }),
    .callback = render3d_raygui_system
  });

}

void flecs_raygui_module_init(ecs_world_t *world){

  rg_register_components(world);
  rg_register_systems(world);

}