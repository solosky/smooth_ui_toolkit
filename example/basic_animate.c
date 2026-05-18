#include <raylib.h>
#include "mc_animate.h"
#include "raylib_helper.h"

typedef struct {
    mc_animate_t anim_x;
    mc_animate_t anim_y;
    float time;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_animate_init_easing(&app->anim_x, MC_FP_C(200), MC_FP_C(600), MC_FP_C(1));
    app->anim_x.repeat = 255;
    app->anim_x.config.easing.easing = mc_ease_cubic_in_out;

    mc_animate_init_easing(&app->anim_y, MC_FP_C(100), MC_FP_C(400), MC_FP_C(0.8f));
    app->anim_y.repeat = 255;
    app->anim_y.config.easing.easing = mc_ease_bounce_out;

    app->time = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    app->time += dt;
    mc_animate_update(&app->anim_x, MC_REAL_FROM_FLOAT(dt));
    mc_animate_update(&app->anim_y, MC_REAL_FROM_FLOAT(dt));

    int x = MC_REAL_TO_INT(app->anim_x.current);
    int y = MC_REAL_TO_INT(app->anim_y.current);

    DrawCircle(x, y, 30, LIGHTGRAY);
    DrawText("Easing: cubic_in_out (x), bounce_out (y)", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Pos: (%d, %d)", x, y), 10, 40, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    mc_example_run(800, 450, "Basic Animate - C99", on_setup, on_draw, &app);
    return 0;
}
