#include <raylib.h>
#include "mc_animate.h"
#include "raylib_helper.h"

typedef struct {
    mc_vec2_animate_t easing_ball;
    mc_vec2_animate_t spring_ball;
    mc_vec2_t easing_target;
    mc_vec2_t spring_target;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;

    mc_vec2_animate_init_easing(&app->easing_ball,
        mc_vec2(MC_FP_C(200), MC_FP_C(150)),
        mc_vec2(MC_FP_C(200), MC_FP_C(150)),
        MC_FP_C(1));
    app->easing_ball.config.easing.easing = mc_ease_cubic_in_out;

    mc_vec2_animate_init_spring(&app->spring_ball,
        mc_vec2(MC_FP_C(200), MC_FP_C(350)),
        mc_vec2(MC_FP_C(200), MC_FP_C(350)));
    app->spring_ball.config.spring.stiffness = MC_FP_C(100);
    app->spring_ball.config.spring.damping = MC_FP_C(10);

    app->easing_target = app->easing_ball.to;
    app->spring_target = app->spring_ball.to;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int mx = GetMouseX();
    int my = GetMouseY();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        mc_vec2_t target = mc_vec2(MC_REAL_FROM_INT(mx), MC_REAL_FROM_INT(my));
        mc_vec2_animate_move_to(&app->easing_ball, target);
        app->easing_target = target;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        mc_vec2_t target = mc_vec2(MC_REAL_FROM_INT(mx), MC_REAL_FROM_INT(my));
        mc_vec2_animate_move_to(&app->spring_ball, target);
        app->spring_target = target;
    }

    mc_vec2_animate_update(&app->easing_ball, MC_REAL_FROM_FLOAT(dt));
    mc_vec2_animate_update(&app->spring_ball, MC_REAL_FROM_FLOAT(dt));

    int ex = MC_REAL_TO_INT(app->easing_ball.current.x);
    int ey = MC_REAL_TO_INT(app->easing_ball.current.y);
    int sx = MC_REAL_TO_INT(app->spring_ball.current.x);
    int sy = MC_REAL_TO_INT(app->spring_ball.current.y);

    ClearBackground(BLACK);

    DrawCircle(ex, ey, 30, BLUE);
    DrawCircleLines(ex, ey, 32, (Color){100, 100, 255, 100});
    DrawText("Easing (Left-click)", ex + 35, ey - 10, 12, LIGHTGRAY);

    DrawCircle(sx, sy, 30, YELLOW);
    DrawCircleLines(sx, sy, 32, (Color){255, 255, 100, 100});
    DrawText("Spring (Right-click)", sx + 35, sy - 10, 12, LIGHTGRAY);

    DrawCircle(MC_REAL_TO_INT(app->easing_target.x),
               MC_REAL_TO_INT(app->easing_target.y), 5, BLUE);
    DrawCircle(MC_REAL_TO_INT(app->spring_target.x),
               MC_REAL_TO_INT(app->spring_target.y), 5, YELLOW);

    DrawText("Vec2 Animation Demo", 10, 10, 20, RAYWHITE);
    DrawText("Left-click: easing ball  |  Right-click: spring ball", 10, 35, 14, DARKGRAY);
    DrawText(TextFormat("Easing pos: (%d, %d)", ex, ey), 10, 60, 14, LIGHTGRAY);
    DrawText(TextFormat("Spring pos: (%d, %d)", sx, sy), 10, 80, 14, LIGHTGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 500, "Vec2 Animation - MotionC", on_setup, on_draw, &app);
    return 0;
}
