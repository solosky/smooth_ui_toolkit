#include <raylib.h>
#include <stdlib.h>
#include "mc_animate.h"
#include "raylib_helper.h"

#define BUBBLE_COUNT 200

typedef struct {
    mc_animate_t x;
    mc_animate_t y;
    int radius;
    Color color;
} bubble_t;

typedef struct {
    bubble_t bubbles[BUBBLE_COUNT];
} app_t;

static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    srand(42);

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        mc_animate_init_spring(&app->bubbles[i].x,
            MC_FP_C(400), MC_FP_C(rand_range(0, 800)));
        mc_animate_init_spring(&app->bubbles[i].y,
            MC_FP_C(225), MC_FP_C(rand_range(0, 450)));

        int stiff = rand_range(50, 150);
        int damp = rand_range(5, 15);
        app->bubbles[i].x.config.spring.stiffness = MC_FP_C(stiff);
        app->bubbles[i].x.config.spring.damping = MC_FP_C(damp);
        app->bubbles[i].y.config.spring = app->bubbles[i].x.config.spring;

        app->bubbles[i].x.to = MC_REAL_FROM_INT(rand_range(0, 800));
        app->bubbles[i].y.to = MC_REAL_FROM_INT(rand_range(0, 450));
        app->bubbles[i].radius = rand_range(3, 6);
        app->bubbles[i].color = mc_example_random_color();
    }
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mc_real_t mx = MC_REAL_FROM_INT(GetMouseX());
        mc_real_t my = MC_REAL_FROM_INT(GetMouseY());
        for (int i = 0; i < BUBBLE_COUNT; i++) {
            app->bubbles[i].x.to = mx;
            app->bubbles[i].y.to = my;
        }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < BUBBLE_COUNT; i++) {
            app->bubbles[i].x.to = MC_REAL_FROM_INT(rand_range(0, 800));
            app->bubbles[i].y.to = MC_REAL_FROM_INT(rand_range(0, 450));
        }
    }

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        mc_animate_update(&app->bubbles[i].x, MC_REAL_FROM_FLOAT(dt));
        mc_animate_update(&app->bubbles[i].y, MC_REAL_FROM_FLOAT(dt));
        int bx = MC_REAL_TO_INT(app->bubbles[i].x.current);
        int by = MC_REAL_TO_INT(app->bubbles[i].y.current);
        DrawCircle(bx, by, app->bubbles[i].radius, app->bubbles[i].color);
    }
}

int main(void) {
    app_t app;
    mc_example_run(800, 450, "Bubbles - C99", on_setup, on_draw, &app);
    return 0;
}
