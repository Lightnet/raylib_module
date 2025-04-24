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
  bool isCaptureMouse;
} RayLibContext;
ECS_COMPONENT_DECLARE(RayLibContext);

typedef enum {
  F_CAMERA_FREE,   // Free camera (WASD, mouse, Space/Ctrl)
  F_CAMERA_PLAYER, // First-person player camera
  F_CAMERA_DEBUG   // Debug camera (orbit with mouse)
} FCameraMode;
ECS_COMPONENT_DECLARE(FCameraMode);

typedef struct {
  FCameraMode currentMode;
} CameraContext_T;
ECS_COMPONENT_DECLARE(CameraContext_T);

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

// Pointer component for raylib Model
typedef struct {
  bool isLoaded;
  Model model;
} ModelComponent;
ECS_COMPONENT_DECLARE(ModelComponent);
// testing
enum FSHAPE {
  FNONE,
  FCUBE,
  FSHPERE,
  FMESH,
  FTERRAIN
};

typedef struct {
  bool isLoaded;
  enum FSHAPE shapeType;
  void* context;
} ShapeComponent;
ECS_COMPONENT_DECLARE(ShapeComponent);

typedef struct {
  Vector3 size;
} CubeComponent;
ECS_COMPONENT_DECLARE(CubeComponent);

typedef struct {
  float radius;
} SphereComponent;
ECS_COMPONENT_DECLARE(SphereComponent);

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
  bool isCaptureMouse;
  bool Left;
  bool Right;
  bool Up;
  bool Down;
  bool isJump;
  float yaw;
  float pitch;
  float mouseSensitivity;
  float moveSpeed;
} PlayerInput_T;
ECS_COMPONENT_DECLARE(PlayerInput_T);

typedef struct {
  int width;
  int height;
} Resize;

ECS_COMPONENT_DECLARE(Resize);

bool is_model_valid(ModelComponent* component);
void flecs_raylib_module_init(ecs_world_t *world);

#endif