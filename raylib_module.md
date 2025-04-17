# raylib module:
  Work in progress.

# Phase:
  Important phase order for onstart and run time render order.

## On Start Phase:

```c
ecs_entity_t OnSetUpPhase;
ecs_entity_t OnSetupGraphicPhase;
ecs_entity_t OnSetupModulePhase;
ecs_entity_t OnSetupWorldPhase;
```
It need to set up window and graphic before any thing else it would crash the application. When using the raylib api if not set up it will crashed.

## Run Time Phase:

```c
ecs_entity_t LogicUpdatePhase;
ecs_entity_t BeginRenderPhase;
//only 3d model render and 2d will not work here.
ecs_entity_t BeginCamera3DPhase;
ecs_entity_t UpdateCamera3DPhase;
ecs_entity_t EndCamera3DPhase;
//only 2d render if 3d will not work here.
ecs_entity_t Render2DPhase;
ecs_entity_t EndRenderPhase;
```

## Shut Down and Clean Up Order:

```c
// shut down and clean up phase
ecs_entity_t ShutDownEvent;
ecs_entity_t ShutDownModule;
ecs_entity_t CleanUpEvent;
ecs_entity_t CleanUpModule;
ecs_entity_t CleanUpGraphicEvent;
ecs_entity_t CleanUpGraphic;
ecs_entity_t CloseEvent;
ecs_entity_t CloseModule;
```