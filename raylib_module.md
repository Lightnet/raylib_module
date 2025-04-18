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

# Set Up format for Transform 3D Hierarchy:
```c
// Transform3D component
typedef struct {
  Vector3 position;                     // Local position
  Quaternion rotation;                  // Local rotation
  Vector3 scale;                        // Local scale
  Matrix localMatrix;                   // Local transform matrix
  Matrix worldMatrix;                   // World transform matrix
  bool isDirty;                         // Flag to indicate if transform needs updating
} Transform3D;
ECS_COMPONENT_DECLARE(Transform3D);
```
  Those are current use for setup. Transform 3D.

  There are local and world transform. Reason is store position, rotation and scale to work correctly from branchs of the node childrens.

  As well isDirty does trigger once if set to true to recalculated the matrix from tree node to reflect change in sub node to update them.

  It would use worldMatrix to set position, rotate, scale for raylib model to render correctly.

# Parent
```c
  ecs_entity_t cube = ecs_entity(it->world, {
    .name = "Cube"
  });
  ecs_set(it->world, cube, Transform3D, {
    .position = (Vector3){0.0f, 2.0f, 0.0f},
    .rotation = QuaternionIdentity(),
    .scale = (Vector3){1.0f, 1.0f, 1.0f},
    .localMatrix = MatrixIdentity(),
    .worldMatrix = MatrixIdentity(),
    .isDirty = true //trigger update once
  });
```
# Child
```c
  ecs_entity_t node2 = ecs_entity(it->world, {
    .name = "NodeChild",
    .parent = cube // parent and attach
  });
  ecs_set(it->world, node2, Transform3D, {
      .position = (Vector3){2.0f, 0.0f, 0.0f},
      .rotation = QuaternionIdentity(),
      .scale = (Vector3){0.5f, 0.5f, 0.5f},
      .localMatrix = MatrixIdentity(),
      .worldMatrix = MatrixIdentity()
  });
```
  For reason added bool isDirty is to do once check update math to reduce recalculated and load if entity need update to not lag that been recalculated already.

# inputs:

```c
void user_input_system(ecs_iter_t *it){
//...
  for (int i = 0; i < it->count; i++) {
    const char *name = ecs_get_name(it->world, it->entities[i]);
    if (name) {
      if (strcmp(name, "Cube") == 0) {
        //...
        bool wasModified = false;
        if (IsKeyDown(KEY_W)) {
          ecs_print(1,"Key W");
          t[i].position.z -= 2.0f * dt;
          wasModified = true;
        }
        //...
        if (wasModified) {
          t[i].isDirty = true;
          printf("Marked %s as dirty\n", name);
        }
      }
    }
  }
}
```
  Althought it required to update the changes else it will not update the change of the parent to child nodes.