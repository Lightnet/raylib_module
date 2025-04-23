// 
// base on dk_console
// this dk_console module

#include <stdio.h>
#include <stdlib.h>
#include <time.h>                   // Required for: time_t, tm, time(), localtime(), strftime()
#include <assert.h>
#include <ctype.h> // isdigit
#include <stdarg.h>
#include <string.h>
#include "flecs_dk_console.h"

#define DK_CONSOLE_EXT_COMMAND_IMPLEMENTATION
#include "dk_command.h"

// static Console console = { .toggle_key = KEY_TAB };
static Console console = { .toggle_key = KEY_GRAVE };
static Console* console_global_ptr = NULL;
static ecs_world_t *c_world; // access to this file only

void CustomLog(int msgType, const char* text, va_list args);

typedef struct {
  double *data;
  int capacity;
  int size;
} Stack;

void Stack_Init(Stack *s) {
  s->capacity = 4;
  s->size = 0;
  s->data = malloc(s->capacity * sizeof(double));
}

void Stack_Push(Stack *s, double value) {
  if (s->size == s->capacity) {
      s->capacity *= 2;
      s->data = realloc(s->data, s->capacity * sizeof(double));
  }
  s->data[s->size++] = value;
}

double Stack_Pop(Stack *s) {
  return s->data[--s->size];
}

int Stack_Empty(Stack *s) {
  return s->size == 0;
}

double evaluateRPN(const char *expr) {
  Stack s;
  Stack_Init(&s);

  while (*expr) {

    while (isspace(*expr)) {
      ++expr;
    }

    if (isdigit(*expr) || ((*expr == '+' || *expr == '-') && isdigit(*(expr + 1)))) {
      char *end;
      double value = strtod(expr, &end);
      expr = end;
      Stack_Push(&s, value);
    } else {
      double right = Stack_Pop(&s);
      double left = Stack_Pop(&s);

      switch (*expr) {
        case '+':
          Stack_Push(&s, left + right);
          break;
        case '-':
          Stack_Push(&s, left - right);
          break;
        case '*':
          Stack_Push(&s, left * right);
          break;
        case '/':
          Stack_Push(&s, left / right);
          break;
        default:
          CustomLog(LOG_ERROR, "Invalid Operation", NULL);
          return 0;
      }

      ++expr;
    }
  }

  double result = Stack_Pop(&s);
  if (!Stack_Empty(&s)) {
    CustomLog(LOG_ERROR, "Invalid expression", NULL);
    return 0;
  }

  return result;
}
// loggin from dk_console, raylib, 
void CustomLog(int msgType, const char* text, va_list args) {
  if (console_global_ptr == NULL || console_global_ptr->logs == NULL) {
      fprintf(stderr, "CustomLog: Invalid console or logs array\n");
      return;
  }
  if (text == NULL) {
      fprintf(stderr, "CustomLog: Text is NULL\n");
      return;
  }

  // Format the log message
  static char buffer[1024] = { 0 };
  vsprintf_s(buffer, sizeof(buffer), text, args);

  char timeStr[64] = { 0 };
  time_t now = time(NULL);
  struct tm* tm_info = localtime(&now);
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);

  char finalBuffer[1024] = { 0 };
  sprintf_s(finalBuffer, sizeof(finalBuffer), "%s %s", timeStr, buffer);

  const char* msgTypeStr = "Unknown";
  switch (msgType) {
      case 2: msgTypeStr = "(Debug)"; break;
      case 3: msgTypeStr = "(Info)"; break;
      case 4: msgTypeStr = "(Warning)"; break;
      case 5: msgTypeStr = "(Error)"; break;
      case 6: msgTypeStr = "(Fatal)"; break;
  }

  char finalBuffer2[1024] = { 0 };
  sprintf_s(finalBuffer2, sizeof(finalBuffer2), "%s %s", msgTypeStr, finalBuffer);

  // Check buffer size
  if (console_global_ptr->log_index >= LOG_SIZE) {
      fprintf(stderr, "CustomLog: Log buffer full, resetting\n");
      for (int i = 0; i < LOG_SIZE; i++) {
          memset(console_global_ptr->logs[i].text, 0, 1024);
      }
      console_global_ptr->log_index = 0;
  }

  // Copy finalBuffer2 into the pre-allocated text buffer
  strncpy(console_global_ptr->logs[console_global_ptr->log_index].text, finalBuffer2, 1023);
  console_global_ptr->logs[console_global_ptr->log_index].text[1023] = '\0';
  console_global_ptr->logs[console_global_ptr->log_index].type = msgType;
  console_global_ptr->log_index++;
}

void echo(const char* argv){

  CustomLog(LOG_INFO, argv, NULL);
}

void clear(const char* argv){

  DK_ConsoleClear(console_global_ptr);
}

void help(const char* args){

  DK_ExtCommand* command_list = DK_ExtGetCommandsList();
  if (args != NULL && strlen(args) > 0) {
    for (int i = 0; i < DK_COMMAND_SIZE; i++)
    {
      if (command_list[i].name != NULL)
      {
        if (strcmp(command_list[i].name, args) == 0)
        {
          CustomLog(LOG_INFO, TextFormat("\t\t`%s`\t\t%s\n", command_list[i].name, command_list[i].help), NULL);
          if (command_list[i].argc > 0) {
            CustomLog(LOG_INFO, TextFormat("\t\ttakes %d argument(s)\n", command_list[i].argc), NULL);
          }
          break;
        }
      }
    }
    return;
  } else {
    CustomLog(LOG_INFO, "******************** HELP ********************", NULL);
    for (int i = 0; i < DK_COMMAND_SIZE; i++)
    {
      if (command_list[i].name != NULL)
      {
        CustomLog(LOG_INFO, TextFormat("\t\t`%s`\t\t%s\n", command_list[i].name, command_list[i].help), NULL);
        if (command_list[i].argc > 0) {
          CustomLog(LOG_INFO, TextFormat("\t\ttakes %d argument(s)\n", command_list[i].argc), NULL);
        }
        CustomLog(LOG_INFO, "----------------------------------------------", NULL);
      }
    }
  }
}

void mathEval(const char* args){

  double result = evaluateRPN(args);
  CustomLog(LOG_INFO, TextFormat("Eval(%s) = %f", args, result), NULL);
}

void feco(const char* argv){
  ecs_print(1,"FLECS TEST...");
  if(c_world){
    ecs_print(1,"world test found...");

    ecs_emit(c_world, &(ecs_event_desc_t) {
      .event = ConsoleEvent,
      .entity = ConsoleModule
    });


  }
  CustomLog(LOG_INFO, argv, NULL);
}

void console_handler(const char* command){

  char* command_buff = (char*)malloc(strlen(command) + 1);
  strcpy(command_buff, command);
  command_buff[strlen(command)] = '\0';

  char* token = strtok(command_buff, " ");

  char* message_buff = (char*)malloc(strlen(command) + 1);
  strcpy(message_buff, command);
  message_buff[strlen(command)] = '\0';

  char* message = strstr(message_buff, token) + strlen(token);
  while (*message == ' ') { message++; }

  if (!DK_ExtCommandExecute(token, message)) {
    CustomLog(LOG_ERROR, TextFormat("Unknown command `%s`", command), NULL);
  }

  free(command_buff);
  free(message_buff);
}

void flecs_dk_console_setup_system(ecs_iter_t *it) {
  ecs_print(1, "flecs_dk_console_setup_system");
  SetTraceLogCallback(CustomLog);
  ecs_print(1, "DK_ExtCommandInit");
  DK_ExtCommandInit();
  DK_ExtCommandPush("echo", 1, "Prints a provided message in the console `echo Hello World`", &echo);
  DK_ExtCommandPush("feco", 1, "flecs test", &feco);
  DK_ExtCommandPush("clear", 0, "Clears the console buffer", &clear);
  DK_ExtCommandPush("help", 1, "Shows the available commands and/or specific one `help <command_name>`", &help);


  console_global_ptr = &console;
  DK_ConsoleInit(console_global_ptr, LOG_SIZE);
  c_world = it->world;

  // Static font to persist
  static Font customFont;
  const char* fontPath = "resources/font/Kenney Pixel.ttf";
  customFont = LoadFont(fontPath);
  if (customFont.texture.id == 0) {
    ecs_print(1, "Failed to load font, falling back to default");
    customFont = GetFontDefault();
  }
  SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);
  ecs_print(1, "Font texture ID: %u", customFont.texture.id);

  ecs_singleton_set(it->world, DKConsoleContext, {
    .imui = {
      .theme = &DK_ImUISolarizedTheme,
      .style = &DK_ImUIDefaultStyle,
      .font = &customFont, // Points to static font
    },
    .console = console_global_ptr,
    // .font = customFont, // Store font object
    .isLoaded = true,
  });

}

void render2d_dk_console_system(ecs_iter_t *it){
  // ecs_print(1,"render2d dk_console");
  DKConsoleContext *dc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if (!dc_ctx || !dc_ctx->isLoaded) return;
  
  // ecs_print(1,"render2d dk_console");

  const char *text = "Press TAB to toggle the console";
  Vector2 position = { 20.0f, 20.0f };
  DrawTextEx(*dc_ctx->imui.font, text, position, 20.0f, 1.0f, GRAY);
  
  // Update console (pass the stored ImUI and console)
  DK_ConsoleUpdate(dc_ctx->console, &dc_ctx->imui, console_handler);

}

void dk_console_event_system(ecs_iter_t *it){
  ecs_print(1,"[event] dk_console_event_system");
}

void dk_console_register_components(ecs_world_t *world){
  // 
  ECS_COMPONENT_DEFINE(world, DKConsoleContext);

  ConsoleEvent = ecs_new(world);
  ConsoleModule = ecs_entity(world, { .name = "ConsoleModule" });
}

void flecs_dk_console_cleanup_system(ecs_iter_t *it) {
  DKConsoleContext *dc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  // ecs_print(1,"flecs_dk_console_cleanup_system");
  // if (dc_ctx && dc_ctx->isLoaded) {
    DK_ConsoleShutdown(dc_ctx->console, LOG_SIZE);
    UnloadFont(*dc_ctx->imui.font);
    // dc_ctx->isLoaded = false;
  // }

  module_break_name(it, "dk_console_module");
  ecs_print(1,"finish clean up dk console.");
}

void flecs_dk_console_cleanup_event_system(ecs_iter_t *it) {
  ecs_print(1,"flecs_dk_console_cleanup_event_system");
  DKConsoleContext *dc_ctx = ecs_singleton_ensure(it->world, DKConsoleContext);
  if(!dc_ctx) return;
  dc_ctx->isLoaded = false;
  // Test
  // for(int i = 0; i < 10000;i++){
  //   ecs_print(1,"test...");  
  // }
  flecs_dk_console_cleanup_system(it);
}

void dk_console_register_systems(ecs_world_t *world){

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "flecs_dk_console_setup_system", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.OnSetupModulePhase)) 
    }),
    .callback = flecs_dk_console_setup_system
  });

  ecs_system_init(world, &(ecs_system_desc_t){
    .entity = ecs_entity(world, { 
        .name = "render2d_dk_console_system", 
        .add = ecs_ids(ecs_dependson(GlobalPhases.Render2D3Phase)) 
    }),
    .callback = render2d_dk_console_system
  });

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = CleanUpModule }},
    .events = { CleanUpEvent },
    .callback = flecs_dk_console_cleanup_event_system
  });

  ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = ConsoleModule }},
    .events = { ConsoleEvent },
    .callback = dk_console_event_system
  });

}
// set up
void flecs_dk_console_module_init(ecs_world_t *world){

  dk_console_register_components(world);

  add_module_name(world, "dk_console_module");

  dk_console_register_systems(world);

}