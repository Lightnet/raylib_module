

#include "flecs_module.h"

void flecs_init_phases(ecs_world_t *world){
  // Define custom phases

  OnSetUpPhase = ecs_new_w_id(world, EcsPhase);
  OnSetupGraphicPhase = ecs_new_w_id(world, EcsPhase);
  OnSetupModulePhase = ecs_new_w_id(world, EcsPhase);
  OnSetupWorldPhase = ecs_new_w_id(world, EcsPhase);

  LogicUpdatePhase = ecs_new_w_id(world, EcsPhase);
  BeginRenderPhase = ecs_new_w_id(world, EcsPhase);
  //only 3d model render and 2d will not work here.
  BeginCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  UpdateCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  EndCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  //only 2d render if 3d will not work here.
  Render2DPhase = ecs_new_w_id(world, EcsPhase);
  EndRenderPhase = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, OnSetUpPhase, EcsDependsOn, EcsOnStart);
  ecs_add_pair(world, OnSetupGraphicPhase, EcsDependsOn, OnSetUpPhase);
  ecs_add_pair(world, OnSetupModulePhase, EcsDependsOn, OnSetupGraphicPhase);
  ecs_add_pair(world, OnSetupWorldPhase, EcsDependsOn, OnSetupModulePhase);

  // Set phase dependencies, must single flow be in order
  ecs_add_pair(world, LogicUpdatePhase, EcsDependsOn, EcsPreUpdate);
  ecs_add_pair(world, BeginRenderPhase, EcsDependsOn, LogicUpdatePhase);
  ecs_add_pair(world, BeginCamera3DPhase, EcsDependsOn, BeginRenderPhase);
  ecs_add_pair(world, UpdateCamera3DPhase, EcsDependsOn, BeginCamera3DPhase);
  ecs_add_pair(world, EndCamera3DPhase, EcsDependsOn, UpdateCamera3DPhase);
  ecs_add_pair(world, Render2DPhase, EcsDependsOn, EndCamera3DPhase);
  ecs_add_pair(world, EndRenderPhase, EcsDependsOn, Render2DPhase);

}

void flecs_register_components(ecs_world_t *world){
  flecs_init_phases(world);

  ShutDownEvent = ecs_new(world);
  ShutDownModule = ecs_entity(world, { .name = "ShutDownModule" });

  CleanUpEvent = ecs_new(world);
  CleanUpModule = ecs_entity(world, { .name = "CleanUpModule" });

  CleanUpGraphicEvent = ecs_new(world);
  CleanUpGraphic = ecs_entity(world, { .name = "CleanUpGraphic" });

  CloseEvent = ecs_new(world);
  CloseModule = ecs_entity(world, { .name = "CloseModule" });

}

void flecs_setup_system(ecs_iter_t *it){
  ecs_print(1,"flecs_setup_system");
}

void flecs_setup_graphic_system(ecs_iter_t *it){
  ecs_print(1,"flecs_setup_graphic_system");
}

void flecs_setup_module_system(ecs_iter_t *it){
  ecs_print(1,"flecs_setup_module_system");
}

//===============================================
// SHUTDOWN > CLEAN // test
//===============================================
void flecs_shutdown_event_system(ecs_iter_t *it){
  ecs_print(1,"[module] flecs_shutdown_event_system");
  ecs_emit(it->world, &(ecs_event_desc_t) {
    .event = CleanUpEvent,
    .entity = CleanUpModule
  });
}
//===============================================
// CLEAN UP > GRAPHIC // test
//===============================================
void flecs_cleanup_event_system(ecs_iter_t *it){
  ecs_print(1,"[module] flecs_cleanup_event_system");
  ecs_emit(it->world, &(ecs_event_desc_t) {
    .event = CleanUpGraphicEvent,
    .entity = CleanUpGraphic
  });
}
//===============================================
// GRAPHIC > CLOSE // test
//===============================================
void flecs_cleanup_graphic_event_system(ecs_iter_t *it){
  ecs_print(1,"[module] flecs_cleanup_graphic_event_system");
  ecs_emit(it->world, &(ecs_event_desc_t) {
    .event = CloseEvent,
    .entity = CloseModule
  });
}
//===============================================
// CLOSE // exit application
//===============================================
void flecs_close_event_system(ecs_iter_t *it){
  ecs_print(1,"[module] flecs_close_event_system");

}

void flecs_register_systems(ecs_world_t *world){

  // Create an entity observer
  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = ShutDownModule }},
    .events = { ShutDownEvent },
    .callback = flecs_shutdown_event_system
  });

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CleanUpModule }},
    .events = { CleanUpEvent },
    .callback = flecs_cleanup_event_system
  });

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CleanUpGraphic }},
    .events = { CleanUpGraphicEvent },
    .callback = flecs_cleanup_graphic_event_system
  });

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CloseModule }},
    .events = { CloseEvent },
    .callback = flecs_close_event_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_setup_system", 
        .add = ecs_ids(ecs_dependson(OnSetUpPhase)) 
    }),
    .callback = flecs_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_setup_graphic_system", 
        .add = ecs_ids(ecs_dependson(OnSetupGraphicPhase)) 
    }),
    .callback = flecs_setup_graphic_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_setup_module_system", 
        .add = ecs_ids(ecs_dependson(OnSetupModulePhase)) 
    }),
    .callback = flecs_setup_module_system
  });
}

void flecs_module_init(ecs_world_t *world){
  ecs_print(1,"init module...");
  ecs_print(1,"flecs_register_components");
  flecs_register_components(world);
  ecs_print(1,"flecs_register_systems");
  flecs_register_systems(world);
  ecs_print(1,"flecs module finish");
}