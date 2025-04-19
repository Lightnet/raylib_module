

#include "flecs_raygui.h"
// #include "flecs_module.h"
// #include <raylib.h>
// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"
// #undef RAYGUI_IMPLEMENTATION


void render3d_raygui_system(ecs_iter_t *it){

  if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")){

    ecs_emit(it->world, &(ecs_event_desc_t) {
      .event = ClickEvent,
      .entity = Widget
    });

  };
}

void OnClick(ecs_iter_t *it){
  ecs_print(1,"CLICK...");
}

void rg_register_components(ecs_world_t *world){
  // 
}

void rg_register_systems(ecs_world_t *world){

  // Create an entity observer
  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = Widget }},
    .events = { ClickEvent },
    .callback = OnClick
  });

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

  ClickEvent = ecs_new(world);
  Widget = ecs_entity(world, { .name = "Widget" });

  rg_register_systems(world);

}