#include <raylib.h>
#include "mc_animate.h"
#include "raylib_helper.h"

typedef struct {
    mc_animate_t anim;
    int direction;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_animate_init_spring(&app->anim, MC_FP_C(200), MC_FP_C(600));
    app->anim.config.spring.stiffness = MC_FP_C(80);
    app->anim.config.spring.damping = MC_FP_C(8);
    app->anim.repeat = 255;
    app->direction = 1;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    mc_animate_update(&app->anim, MC_REAL_FROM_FLOAT(dt));

    mc_real_t dist = app->anim.to - app->anim.from;
    if (dist < 0) dist = -dist;
    if (dist < MC_FP_C(5)) {
        app->direction = -app->direction;
        app->anim.from = app->anim.current;
        app->anim.to = app->direction > 0 ? MC_FP_C(600) : MC_FP_C(200);
        app->anim.velocity = 0;
    }

    int x = MC_REAL_TO_INT(app->anim.current);
    DrawCircle(x, 225, 30, LIGHTGRAY);
    DrawText(TextFormat("x: %d", x), 10, 10, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    mc_example_run(800, 450, "Animate - C99", on_setup, on_draw, &app);
    return 0;
}
