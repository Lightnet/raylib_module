#include "flecs.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void delta_time_input_system(ecs_iter_t *it) {
    ecs_print(1, "delta_time %.8f", it->delta_time);
}

int main() {
    ecs_world_t *world = ecs_init();

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "delta_time_input_system", .add = ecs_ids(ecs_dependson(EcsOnUpdate)) }),
        .callback = delta_time_input_system
    });

    LARGE_INTEGER frequency, last_time, current_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last_time);
    float delta_time = 0.0f;
    const float target_frame_time = 1.0f / 60.0f; // Target 60 FPS
    const float fixed_time_step = 1.0f / 60.0f; // Fixed step
    float accumulated_time = 0.0f;
    int count = 0;
    bool shouldQuit = false;

    while (!shouldQuit) {
        QueryPerformanceCounter(&last_time);
        delta_time = (float)(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;
        last_time = current_time;

        if (delta_time > 0.1f) delta_time = 0.1f;

        accumulated_time += delta_time;

        while (accumulated_time >= fixed_time_step) {
            ecs_progress(world, fixed_time_step);
            ecs_print(1, "loop delta_time %.8f", fixed_time_step);
            accumulated_time -= fixed_time_step;
        }

        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);
        float frame_time = (float)(frame_end_time.QuadPart - current_time.QuadPart) / frequency.QuadPart;
        float sleep_time = target_frame_time - frame_time;
        if (sleep_time > 0) {
            Sleep((DWORD)(sleep_time * 1000.0f));
        }

        count++;
        if (count > 1000) {
            shouldQuit = true;
        }
    }

    ecs_fini(world);
    ecs_print(1, "finish\n");

    return 0;
}