// To set up flecs module needed for handle on start set up and run time update order.

#include "flecs_module.h"

FlecsPhases GlobalPhases = {0};

void flecs_init_phases(ecs_world_t *world, FlecsPhases *phases){
  // Define custom phases

  phases->OnSetUpPhase = ecs_new_w_id(world, EcsPhase);
  phases->OnSetupGraphicPhase = ecs_new_w_id(world, EcsPhase);
  phases->OnSetupModulePhase = ecs_new_w_id(world, EcsPhase);
  phases->OnSetupWorldPhase = ecs_new_w_id(world, EcsPhase);

  phases->LogicUpdatePhase = ecs_new_w_id(world, EcsPhase);
  phases->BeginRenderPhase = ecs_new_w_id(world, EcsPhase);
  //only 3d model render and 2d will not work here.
  phases->BeginCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  phases->UpdateCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  phases->EndCamera3DPhase = ecs_new_w_id(world, EcsPhase);
  //only 2d render if 3d will not work here.
  phases->Render2D1Phase = ecs_new_w_id(world, EcsPhase);
  phases->Render2D2Phase = ecs_new_w_id(world, EcsPhase);
  phases->Render2D3Phase = ecs_new_w_id(world, EcsPhase);
  phases->EndRenderPhase = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, phases->OnSetUpPhase, EcsDependsOn, EcsOnStart);
  ecs_add_pair(world, phases->OnSetupGraphicPhase, EcsDependsOn, phases->OnSetUpPhase);
  ecs_add_pair(world, phases->OnSetupModulePhase, EcsDependsOn, phases->OnSetupGraphicPhase);
  ecs_add_pair(world, phases->OnSetupWorldPhase, EcsDependsOn, phases->OnSetupModulePhase);

  // Set phase dependencies, must single flow be in order
  ecs_add_pair(world, phases->LogicUpdatePhase, EcsDependsOn, EcsPreUpdate);
  ecs_add_pair(world, phases->BeginRenderPhase, EcsDependsOn, phases->LogicUpdatePhase);
  ecs_add_pair(world, phases->BeginCamera3DPhase, EcsDependsOn, phases->BeginRenderPhase);
  ecs_add_pair(world, phases->UpdateCamera3DPhase, EcsDependsOn, phases->BeginCamera3DPhase);
  ecs_add_pair(world, phases->EndCamera3DPhase, EcsDependsOn, phases->UpdateCamera3DPhase);
  ecs_add_pair(world, phases->Render2D1Phase, EcsDependsOn, phases->EndCamera3DPhase);
  ecs_add_pair(world, phases->Render2D2Phase, EcsDependsOn, phases->Render2D1Phase);
  ecs_add_pair(world, phases->Render2D3Phase, EcsDependsOn, phases->Render2D2Phase);
  ecs_add_pair(world, phases->EndRenderPhase, EcsDependsOn, phases->Render2D3Phase);

}

void flecs_register_components(ecs_world_t *world){

  

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
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetUpPhase)) 
    }),
    .callback = flecs_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_setup_graphic_system", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetupGraphicPhase)) 
    }),
    .callback = flecs_setup_graphic_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_setup_module_system", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetupModulePhase)) 
    }),
    .callback = flecs_setup_module_system
  });
}

void flecs_module_init(ecs_world_t *world){
  ecs_print(1,"init module...");
  flecs_init_phases(world, &GlobalPhases);
  ecs_print(1,"flecs_register_components");
  flecs_register_components(world);
  ecs_print(1,"flecs_register_systems");
  flecs_register_systems(world);
  ecs_print(1,"flecs module finish");
}