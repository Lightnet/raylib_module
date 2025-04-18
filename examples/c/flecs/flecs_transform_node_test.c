#include "flecs.h"
#include <stdio.h>

// Transform3D component
typedef struct {
    float position[3];  // Local position (x, y, z)
    float worldPos[3];  // World position
    bool isDirty;       // Dirty flag for optimization
} Transform3D;

// Declare component
ECS_COMPONENT_DECLARE(Transform3D);

// Function to update transforms
void UpdateTransform(ecs_world_t *world, ecs_entity_t entity, Transform3D *transform) {
    ecs_entity_t parent = ecs_get_parent(world, entity);
    const char *name = ecs_get_name(world, entity) ? ecs_get_name(world, entity) : "(unnamed)";
    bool parentIsDirty = false;

    // Check if parent is dirty
    if (parent && ecs_is_valid(world, parent)) {
        const Transform3D *parent_transform = ecs_get(world, parent, Transform3D);
        if (parent_transform && parent_transform->isDirty) {
            parentIsDirty = true;
        }
    }

    // Skip update if neither this transform nor its parent is dirty
    if (!transform->isDirty && !parentIsDirty) {
        return;
    }

    // Update position for root entity
    if (!parent || !ecs_is_valid(world, parent)) {
        transform->position[0] += 0.1f;  // Move right each frame
        transform->worldPos[0] = transform->position[0];
        transform->worldPos[1] = transform->position[1];
        transform->worldPos[2] = transform->position[2];
        printf("Root %s position (%.2f, %.2f, %.2f)\n",
               name, transform->worldPos[0], transform->worldPos[1], transform->worldPos[2]);
    } else {
        // Child: world position = local position + parent world position
        const Transform3D *parent_transform = ecs_get(world, parent, Transform3D);
        if (!parent_transform) {
            printf("Error: Parent %s lacks Transform3D for %s\n",
                   ecs_get_name(world, parent), name);
            return;
        }
        transform->worldPos[0] = transform->position[0] + parent_transform->worldPos[0];
        transform->worldPos[1] = transform->position[1] + parent_transform->worldPos[1];
        transform->worldPos[2] = transform->position[2] + parent_transform->worldPos[2];
        const char *parent_name = ecs_get_name(world, parent) ? ecs_get_name(world, parent) : "(unnamed)";
        printf("Child %s position (%.2f, %.2f, %.2f), parent %s world pos (%.2f, %.2f, %.2f), world pos (%.2f, %.2f, %.2f)\n",
               name, transform->position[0], transform->position[1], transform->position[2],
               parent_name, parent_transform->worldPos[0], parent_transform->worldPos[1], parent_transform->worldPos[2],
               transform->worldPos[0], transform->worldPos[1], transform->worldPos[2]);
    }

    // Mark children as dirty
    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            Transform3D *child_transform = ecs_get_mut(world, it.entities[i], Transform3D);
            if (child_transform) {
                child_transform->isDirty = true;
                // Use initializer list for ecs_set
                ecs_set(world, it.entities[i], Transform3D, {
                    .position = {child_transform->position[0], child_transform->position[1], child_transform->position[2]},
                    .worldPos = {child_transform->worldPos[0], child_transform->worldPos[1], child_transform->worldPos[2]},
                    .isDirty = child_transform->isDirty
                });
            }
        }
    }

    transform->isDirty = false;
}

// Function to process entity hierarchy
void ProcessEntityHierarchy(ecs_world_t *world, ecs_entity_t entity) {
    Transform3D *transform = ecs_get_mut(world, entity, Transform3D);
    if (transform) {
        UpdateTransform(world, entity, transform);
        // Use initializer list for ecs_set
        ecs_set(world, entity, Transform3D, {
            .position = {transform->position[0], transform->position[1], transform->position[2]},
            .worldPos = {transform->worldPos[0], transform->worldPos[1], transform->worldPos[2]},
            .isDirty = transform->isDirty
        });
    }

    // Process children
    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            ProcessEntityHierarchy(world, it.entities[i]);
        }
    }
}

// System to update transform hierarchy
void UpdateTransformHierarchySystem(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t entity = it->entities[i];
        ProcessEntityHierarchy(it->world, entity);
    }
}

int main(void) {
    // Initialize Flecs world
    ecs_world_t *world = ecs_init();

    // Define Transform3D component
    ECS_COMPONENT_DEFINE(world, Transform3D);

    // Register system for root entities (no parent)
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "UpdateTransformHierarchySystem" }),
        .query.terms = {
            { .id = ecs_id(Transform3D), .src.id = EcsSelf },
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .callback = UpdateTransformHierarchySystem
    });

    // Create parent entity
    ecs_entity_t parent = ecs_entity(world, { .name = "NodeParent" });
    ecs_set(world, parent, Transform3D, {
        .position = {0.0f, 0.0f, 0.0f},
        .worldPos = {0.0f, 0.0f, 0.0f},
        .isDirty = true
    });

    // Create child entity
    ecs_entity_t child = ecs_entity(world, { .name = "NodeChild", .parent = parent });
    ecs_set(world, child, Transform3D, {
        .position = {2.0f, 0.0f, 0.0f},
        .worldPos = {2.0f, 0.0f, 0.0f},
        .isDirty = true
    });

    // Run for 10 frames
    for (int i = 0; i < 10; i++) {
        printf("Frame %d:\n", i + 1);
        ecs_progress(world, 0);
        printf("\n");
    }

    // Clean up
    ecs_fini(world);
    return 0;
}