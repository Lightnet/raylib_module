#ifndef FLECS_DK_CONSOLE_H
#define FLECS_DK_CONSOLE_H

// #include <time.h>
// #include <assert.h>
// #include <ctype.h>
// #include <stdio.h>

#include "flecs_module.h"
#include "flecs_raylib.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

#define DK_CONSOLE_IMPLEMENTATION
#include "dk_console.h"
// #undef DK_CONSOLE_IMPLEMENTATION

typedef struct {
  ImUI imui;
  Console* console;
  Font font;
  bool isLoaded;
} DKConsoleContext;

ECS_COMPONENT_DECLARE(DKConsoleContext);

void flecs_dk_console_module_init(ecs_world_t *world);

#endif