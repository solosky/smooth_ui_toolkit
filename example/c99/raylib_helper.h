#ifndef RAYLIB_HELPER_H
#define RAYLIB_HELPER_H

#include <raylib.h>

typedef void (*sut_example_draw_fn)(void* ctx);
typedef void (*sut_example_setup_fn)(void* ctx);

void sut_example_run(int width, int height, const char* title,
                     sut_example_setup_fn on_setup,
                     sut_example_draw_fn on_draw,
                     void* ctx);

Color sut_example_random_color(void);

#endif
