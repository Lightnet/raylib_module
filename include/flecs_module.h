#ifndef FLECS_MODULE_H
#define FLECS_MODULE_H

#include "flecs.h"
#include "raylib.h"

typedef struct {
  char name[32]; // Fixed-size string for simplicity
  bool isCleanUp;
} PluginModule;
ECS_COMPONENT_DECLARE(PluginModule);

ecs_entity_t OnSetUpPhase;
ecs_entity_t OnSetupGraphicPhase;
ecs_entity_t OnSetupModulePhase;

ecs_entity_t LogicUpdatePhase;
ecs_entity_t BeginRenderPhase;
//only 3d model render and 2d will not work here.
ecs_entity_t BeginCamera3DPhase;
ecs_entity_t UpdateCamera3DPhase;
ecs_entity_t EndCamera3DPhase;
//only 2d render if 3d will not work here.
ecs_entity_t Render2DPhase;
ecs_entity_t EndRenderPhase;
// shut down and clean up phase
ecs_entity_t ShutDownEvent;
ecs_entity_t ShutDownModule;
ecs_entity_t CleanUpEvent;
ecs_entity_t CleanUpModule;
ecs_entity_t CleanUpGraphicEvent;
ecs_entity_t CleanUpGraphic;
ecs_entity_t CloseEvent;
ecs_entity_t CloseModule;
// init module
void flecs_module_init(ecs_world_t *world);


#endif