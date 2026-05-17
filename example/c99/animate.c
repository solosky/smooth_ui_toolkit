#include <raylib.h>
#include "sut_animate.h"
#include "raylib_helper.h"

typedef struct {
    sut_animate_t anim;
    int direction;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    sut_animate_init_spring(&app->anim, SUT_FP_C(200), SUT_FP_C(600));
    app->anim.config.spring.stiffness = SUT_FP_C(80);
    app->anim.config.spring.damping = SUT_FP_C(8);
    app->anim.repeat = 255;
    app->direction = 1;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    sut_animate_update(&app->anim, SUT_REAL_FROM_FLOAT(dt));

    sut_real_t dist = app->anim.to - app->anim.from;
    if (dist < 0) dist = -dist;
    if (dist < SUT_FP_C(5)) {
        app->direction = -app->direction;
        app->anim.from = app->anim.current;
        app->anim.to = app->direction > 0 ? SUT_FP_C(600) : SUT_FP_C(200);
        app->anim.velocity = 0;
    }

    int x = SUT_REAL_TO_INT(app->anim.current);
    DrawCircle(x, 225, 30, LIGHTGRAY);
    DrawText(TextFormat("x: %d", x), 10, 10, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Animate - C99", on_setup, on_draw, &app);
    return 0;
}
