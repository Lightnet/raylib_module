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
ecs_entity_t Render2D1Phase;
ecs_entity_t Render2D2Phase;
ecs_entity_t Render2D3Phase;
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

# Parent and Child Relationships:
 * https://www.flecs.dev/flecs/md_docs_2Relationships.html
  
  The reason for Transform hierarchy in parent and child is relate how flecs format for query and loop to handle matrix and children. As well it handle model parent child to update their position correctly.

  Flecs has two basic relationships and pairing. Flecs as two or more ways. One way using the flecs default relationships pairing. The other is customize pairing entity. But be working on parent and child entity pairing.

## Method 1
```c
// Create two entities with a parent/child name
ecs_entity_t parent = ecs_entity(world, {
  .name = "Parent"
});
 
ecs_entity_t child = ecs_entity(world, {
  .name = "Child"
});
 
// Create the hierarchy
ecs_add_pair(world, child, EcsChildOf, parent);
```

```c
ecs_remove_pair(world, child, EcsChildOf, parent);
```
  By using the default EcsChildOf which help reduce adding custem entity to add and remove pairs.

## Transform hierarchy update:
  There will be bool isDirty for update matrix when first time init else it position zero by default. For parent it need set isDirty true to update the world matrix. It to reduce recalculated if the entity stay same position if not move to prevent recalculated matrix branching node children update matrix.

## Method 2
```c
  ecs_entity_t cube = ecs_entity(it->world, {
    .name = "Cube"
  });

  ecs_entity_t node2 = ecs_entity(it->world, {
    .name = "NodeChild",
    .parent = cube // parent
  });
```


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

# GUI:
 None

# References:
 * 