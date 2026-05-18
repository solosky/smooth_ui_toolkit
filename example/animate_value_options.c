#include <raylib.h>
#include "mc_animate.h"
#include "raylib_helper.h"

typedef struct {
    mc_animate_t anim_x;
    mc_animate_t anim_y;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_animate_init_spring(&app->anim_x, MC_FP_C(100), MC_FP_C(100));
    app->anim_x.config.spring.stiffness = MC_FP_C(180);
    app->anim_x.config.spring.damping = MC_FP_C(15);
    mc_animate_init_spring(&app->anim_y, MC_FP_C(225), MC_FP_C(225));
    app->anim_y.config.spring = app->anim_x.config.spring;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        app->anim_x.from = app->anim_x.current;
        app->anim_x.to = MC_REAL_FROM_INT(GetMouseX());
        app->anim_x.velocity = 0;
        app->anim_y.from = app->anim_y.current;
        app->anim_y.to = MC_REAL_FROM_INT(GetMouseY());
        app->anim_y.velocity = 0;
    }

    mc_animate_update(&app->anim_x, MC_REAL_FROM_FLOAT(dt));
    mc_animate_update(&app->anim_y, MC_REAL_FROM_FLOAT(dt));

    int x = MC_REAL_TO_INT(app->anim_x.current);
    int y = MC_REAL_TO_INT(app->anim_y.current);

    ClearBackground(BLACK);
    DrawText("Click To Move The Ball", 280, 200, 20, DARKGRAY);
    DrawCircle(x, y, 30, LIGHTGRAY);
    DrawText(TextFormat("Spring(stiff=%d, damp=%d)",
        MC_REAL_TO_INT(app->anim_x.config.spring.stiffness),
        MC_REAL_TO_INT(app->anim_x.config.spring.damping)),
        10, 10, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    mc_example_run(800, 450, "Animate Value Options - C99", on_setup, on_draw, &app);
    return 0;
}
