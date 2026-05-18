#include <raylib.h>
#include <stdlib.h>
#include "sut_animate.h"
#include "raylib_helper.h"

#define BUBBLE_COUNT 200

typedef struct {
    sut_animate_t x;
    sut_animate_t y;
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
        sut_animate_init_spring(&app->bubbles[i].x,
            SUT_FP_C(400), SUT_FP_C(rand_range(0, 800)));
        sut_animate_init_spring(&app->bubbles[i].y,
            SUT_FP_C(225), SUT_FP_C(rand_range(0, 450)));

        int stiff = rand_range(50, 150);
        int damp = rand_range(5, 15);
        app->bubbles[i].x.config.spring.stiffness = SUT_FP_C(stiff);
        app->bubbles[i].x.config.spring.damping = SUT_FP_C(damp);
        app->bubbles[i].y.config.spring = app->bubbles[i].x.config.spring;

        app->bubbles[i].x.to = SUT_REAL_FROM_INT(rand_range(0, 800));
        app->bubbles[i].y.to = SUT_REAL_FROM_INT(rand_range(0, 450));
        app->bubbles[i].radius = rand_range(3, 6);
        app->bubbles[i].color = sut_example_random_color();
    }
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        sut_real_t mx = SUT_REAL_FROM_INT(GetMouseX());
        sut_real_t my = SUT_REAL_FROM_INT(GetMouseY());
        for (int i = 0; i < BUBBLE_COUNT; i++) {
            app->bubbles[i].x.to = mx;
            app->bubbles[i].y.to = my;
        }
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < BUBBLE_COUNT; i++) {
            app->bubbles[i].x.to = SUT_REAL_FROM_INT(rand_range(0, 800));
            app->bubbles[i].y.to = SUT_REAL_FROM_INT(rand_range(0, 450));
        }
    }

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        sut_animate_update(&app->bubbles[i].x, SUT_REAL_FROM_FLOAT(dt));
        sut_animate_update(&app->bubbles[i].y, SUT_REAL_FROM_FLOAT(dt));
        int bx = SUT_REAL_TO_INT(app->bubbles[i].x.current);
        int by = SUT_REAL_TO_INT(app->bubbles[i].y.current);
        DrawCircle(bx, by, app->bubbles[i].radius, app->bubbles[i].color);
    }
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Bubbles - C99", on_setup, on_draw, &app);
    return 0;
}
