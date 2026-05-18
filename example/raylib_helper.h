#ifndef RAYLIB_HELPER_H
#define RAYLIB_HELPER_H

#include <raylib.h>

typedef void (*mc_example_draw_fn)(void* ctx);
typedef void (*mc_example_setup_fn)(void* ctx);

void mc_example_run(int width, int height, const char* title,
                     mc_example_setup_fn on_setup,
                     mc_example_draw_fn on_draw,
                     void* ctx);

Color mc_example_random_color(void);

#endif
