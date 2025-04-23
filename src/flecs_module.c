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

  ECS_COMPONENT_DEFINE(world, PluginModule);
  ECS_COMPONENT_DEFINE(world, ModuleContext);

  ShutDownEvent = ecs_new(world);
  ShutDownModule = ecs_entity(world, { .name = "ShutDownModule" });

  CleanUpEvent = ecs_new(world);
  CleanUpModule = ecs_entity(world, { .name = "CleanUpModule" });

  CleanUpGraphicEvent = ecs_new(world);
  CleanUpGraphic = ecs_entity(world, { .name = "CleanUpGraphic" });

  CloseEvent = ecs_new(world);
  CloseModule = ecs_entity(world, { .name = "CloseModule" });

  ClickEvent = ecs_new(world);
  Widget = ecs_entity(world, { .name = "Widget" });

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

void flecs_cleanup_checks_system(ecs_iter_t *it){
  ModuleContext *g_module = ecs_singleton_ensure(it->world, ModuleContext);
  if(!g_module)return;
  PluginModule *plugin = ecs_field(it, PluginModule, 0);
  //ecs_print(1,"checking...");

  ecs_query_t *q = ecs_query(it->world, {
    .terms = {
      { .id = ecs_id(PluginModule) },
    }
  });

  ecs_iter_t s_it = ecs_query_iter(it->world, q);

  bool isFinishCleanup = false;
  int count = 0;
  while (ecs_query_next(&s_it)) {
    PluginModule *p = ecs_field(&s_it, PluginModule, 0);
    for (int i = 0; i < s_it.count; i ++) {
      // ecs_print(1,"Module Name : %s", 
      //   p[i].name
      //   //ecs_get_name(s_it.world, s_it.entities[i])
      // );
      //ecs_print(1,"isCleanUp %d", p[i].isCleanUp);
      if(p[i].isCleanUp){
        isFinishCleanup = true;
        count+=1;
      }
    }
  }

  //
  ecs_print(1," COUNT: %d Len:%d ", count, s_it.count);
  if((count == s_it.count && g_module->isCleanUpModule == false) ){
    g_module->isCleanUpModule = true;
    ecs_print(1,"CLOSE......");
    ecs_print(1,"CLOSE......");
    ecs_emit(it->world, &(ecs_event_desc_t) {
      .event = CleanUpGraphicEvent,
      .entity = CleanUpGraphic
    });
  }

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
// void flecs_cleanup_event_system(ecs_iter_t *it){
  // ecs_print(1,"[module] flecs_cleanup_event_system");
  // ecs_emit(it->world, &(ecs_event_desc_t) {
  //   .event = CleanUpGraphicEvent,
  //   .entity = CleanUpGraphic
  // });
// }
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

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
      .name = "flecs_cleanup_checks_system", 
      .add = ecs_ids(ecs_dependson(GlobalPhases.LogicUpdatePhase)) 
    }),
    .query.terms = {{ ecs_id(PluginModule), .inout = EcsIn }},
    .callback = flecs_cleanup_checks_system
  });

  // Create an entity observer
  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = ShutDownModule }},
    .events = { ShutDownEvent },
    .callback = flecs_shutdown_event_system
  });

  // ecs_observer(world, {
  //   // Not interested in any specific component
  //   .query.terms = {{ EcsAny, .src.id = CleanUpModule }},
  //   .events = { CleanUpEvent },
  //   .callback = flecs_cleanup_event_system
  // });

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
//===============================================
// INIT MODULE
//===============================================
void flecs_module_init(ecs_world_t *world){
  ecs_print(1,"init module...");
  flecs_init_phases(world, &GlobalPhases);
  ecs_print(1,"flecs_register_components");
  flecs_register_components(world);
  ecs_print(1,"flecs_register_systems");
  flecs_register_systems(world);
  ecs_print(1,"flecs module finish");

  ecs_singleton_set(world, ModuleContext, {
    .isCleanUpModule=false,
    .moduleCount=0
  });
}
//===============================================
// ADD MODULE NAME
//===============================================
ecs_entity_t add_module_name(ecs_world_t *world, const char *name) {
  ecs_entity_t e = ecs_new(world);
  PluginModule module = { .isCleanUp = false };
  
  // Copy name safely
  #ifdef _MSC_VER
      strncpy_s(module.name, sizeof(module.name), name, _TRUNCATE); // MSVC-safe
  #else
      strncpy(module.name, name, sizeof(module.name) - 1);
      module.name[sizeof(module.name) - 1] = '\0'; // Ensure null-termination
  #endif
  
  // Set component
  //ecs_set(world, e, PluginModule, module);
  ecs_set_id(world, e, ecs_id(PluginModule), sizeof(PluginModule), &module);
  return e;
}

//===============================================
// ADD MODULE NAME
//===============================================
void module_break_name(ecs_iter_t *it, const char *module_name){
  // Create query for PluginModule
  ecs_query_t *q = ecs_query(it->world, {
    .terms = {
      { .id = ecs_id(PluginModule) }
    }
  });

  ecs_iter_t s_it = ecs_query_iter(it->world, q);
  bool found = false;

  while (ecs_query_next(&s_it) && !found) {
    PluginModule *p = ecs_field(&s_it, PluginModule, 0); // 1-based index
    for (int i = 0; i < s_it.count; i++) {
        printf("Checking module: %s\n", p[i].name);
        if (strcmp(p[i].name, module_name) == 0) {
            p[i].isCleanUp = true;
            printf("Marked %s for cleanup\n", module_name);
            found = true;
            break; // Exit inner loop
        }
    }
  }
  
}
