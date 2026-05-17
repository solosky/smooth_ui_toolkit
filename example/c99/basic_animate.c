#include <raylib.h>
#include "sut_animate.h"
#include "raylib_helper.h"

typedef struct {
    sut_animate_t anim_x;
    sut_animate_t anim_y;
    float time;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    sut_animate_init_easing(&app->anim_x, SUT_FP_C(200), SUT_FP_C(600), SUT_FP_C(1));
    app->anim_x.repeat = 255;
    app->anim_x.config.easing.easing = sut_ease_cubic_in_out;

    sut_animate_init_easing(&app->anim_y, SUT_FP_C(100), SUT_FP_C(400), SUT_FP_C(0.8f));
    app->anim_y.repeat = 255;
    app->anim_y.config.easing.easing = sut_ease_bounce_out;

    app->time = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    app->time += dt;
    sut_animate_update(&app->anim_x, SUT_REAL_FROM_FLOAT(dt));
    sut_animate_update(&app->anim_y, SUT_REAL_FROM_FLOAT(dt));

    int x = SUT_REAL_TO_INT(app->anim_x.current);
    int y = SUT_REAL_TO_INT(app->anim_y.current);

    DrawCircle(x, y, 30, LIGHTGRAY);
    DrawText("Easing: cubic_in_out (x), bounce_out (y)", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Pos: (%d, %d)", x, y), 10, 40, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Basic Animate - C99", on_setup, on_draw, &app);
    return 0;
}
