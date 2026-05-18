#include "raylib_helper.h"
#include <stdlib.h>

void mc_example_run(int width, int height, const char* title,
                     mc_example_setup_fn on_setup,
                     mc_example_draw_fn on_draw,
                     void* ctx)
{
    InitWindow(width, height, title);
    SetTargetFPS(60);
    if (on_setup) on_setup(ctx);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (on_draw) on_draw(ctx);
        EndDrawing();
    }
    CloseWindow();
}

Color mc_example_random_color(void)
{
    Color c;
    c.r = GetRandomValue(60, 255);
    c.g = GetRandomValue(60, 255);
    c.b = GetRandomValue(60, 255);
    c.a = 255;
    return c;
}
