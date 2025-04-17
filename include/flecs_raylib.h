#ifndef FLECS_RAYLIB_H
#define FLECS_RAYLIB_H

#include "flecs.h"
#include "raylib.h"
#include "raymath.h" // For quaternion and matrix operations
#include "rlgl.h"    // For rlPushMatrix, rlTranslatef, etc.

typedef struct {
  Camera3D camera;
  bool isCameraValid;
  bool shouldQuit;
  bool hasError;
  bool isShutDown;
  int width;
  int height;
  bool isLoaded;
} RayLibContext;

ECS_COMPONENT_DECLARE(RayLibContext);

// Transform3D component
typedef struct {
  Vector3 position;
  Quaternion rotation;
  Vector3 scale;
  Matrix localMatrix;
  Matrix worldMatrix;
} Transform3D;
ECS_COMPONENT_DECLARE(Transform3D);

// Pointer component for raylib Model
typedef struct {
  Model model;
  bool isLoaded;
} ModelComponent;
ECS_COMPONENT_DECLARE(ModelComponent);

typedef struct {
  Model* model;
} PHComponent;
ECS_COMPONENT_DECLARE(PHComponent);

Model g_model;


void flecs_raylib_module_init(ecs_world_t *world);


#endif