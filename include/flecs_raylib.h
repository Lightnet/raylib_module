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
  bool isLoaded;
  Model model;
} ModelComponent;
ECS_COMPONENT_DECLARE(ModelComponent);
// test
typedef struct {
  Model* model;
} PHComponent;
ECS_COMPONENT_DECLARE(PHComponent);
// test
Model g_model;

typedef struct {
  bool pressed;
  bool state;
  bool current;
} ECS_RL_KEY_STATE_T;
ECS_COMPONENT_DECLARE(ECS_RL_KEY_STATE_T);

typedef struct {
  float x;
  float y;
  float xrel;
  float yrel;
} ECS_RL_MOTION_T;

typedef struct {
  float x;
  float y;
} ECS_RL_WHEEL_T;

typedef struct {
  ECS_RL_KEY_STATE_T keys[340];
  // ECS_SDL_MOUSE_T mouse;
  // SDL_Event event;
} ECS_RL_INPUT_T;
ECS_COMPONENT_DECLARE(ECS_RL_INPUT_T);

typedef struct {
  bool isMovementMode;
  bool tabPressed;
} PlayerInput_T;
ECS_COMPONENT_DECLARE(PlayerInput_T);

bool is_model_valid(ModelComponent* component);

void flecs_raylib_module_init(ecs_world_t *world);

#endif