#include <raylib.h>
#include "sut_animate.h"
#include "raylib_helper.h"

typedef struct {
    sut_animate_t anim_x;
    sut_animate_t anim_y;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    sut_animate_init_spring(&app->anim_x, SUT_FP_C(400), SUT_FP_C(400));
    sut_animate_init_spring(&app->anim_y, SUT_FP_C(225), SUT_FP_C(225));
    app->anim_x.config.spring.stiffness = SUT_FP_C(120);
    app->anim_x.config.spring.damping = SUT_FP_C(12);
    app->anim_y.config.spring = app->anim_x.config.spring;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        app->anim_x.to = SUT_REAL_FROM_INT(GetMouseX());
        app->anim_y.to = SUT_REAL_FROM_INT(GetMouseY());
        app->anim_x.from = app->anim_x.current;
        app->anim_y.from = app->anim_y.current;
        app->anim_x.velocity = 0;
        app->anim_y.velocity = 0;
        app->anim_x.elapsed = 0;
        app->anim_y.elapsed = 0;
    }

    sut_animate_update(&app->anim_x, SUT_REAL_FROM_FLOAT(dt));
    sut_animate_update(&app->anim_y, SUT_REAL_FROM_FLOAT(dt));

    int x = SUT_REAL_TO_INT(app->anim_x.current);
    int y = SUT_REAL_TO_INT(app->anim_y.current);

    DrawCircle(x, y, 30, LIGHTGRAY);
    DrawText("Click To Move The Ball", 280, 200, 20, DARKGRAY);
    DrawText(TextFormat("Pos: (%d, %d)", x, y), 10, 10, 16, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Animate Value - C99", on_setup, on_draw, &app);
    return 0;
}
