#include <raylib.h>
#include <string.h>
#include "mc_spring.h"
#include "raylib_helper.h"

typedef struct {
    mc_real_t x;
    mc_real_t v;
    mc_real_t target_x;
    mc_spring_params_t params;
    int grabbed;
    int show_help;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->x = MC_FP_C(400);
    app->v = 0;
    app->target_x = MC_FP_C(400);
    app->params.stiffness = MC_FP_C(80);
    app->params.damping = MC_FP_C(8);
    app->params.mass = MC_FP_C(1);
    app->grabbed = 0;
    app->show_help = 1;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int mx = GetMouseX();
    int my = GetMouseY();

    if (IsKeyPressed(KEY_H)) app->show_help = !app->show_help;

    if (IsKeyDown(KEY_UP)) {
        app->params.stiffness += MC_FP_C(2);
        if (app->params.stiffness > MC_FP_C(500))
            app->params.stiffness = MC_FP_C(500);
    }
    if (IsKeyDown(KEY_DOWN)) {
        app->params.stiffness -= MC_FP_C(2);
        if (app->params.stiffness < MC_FP_C(1))
            app->params.stiffness = MC_FP_C(1);
    }
    if (IsKeyDown(KEY_LEFT)) {
        app->params.damping -= MC_FP_C(1);
        if (app->params.damping < MC_FP_C(1))
            app->params.damping = MC_FP_C(1);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        app->params.damping += MC_FP_C(1);
        if (app->params.damping > MC_FP_C(50))
            app->params.damping = MC_FP_C(50);
    }
    if (IsKeyDown(KEY_A)) {
        app->params.mass += MC_FP_C(0.5f);
        if (app->params.mass > MC_FP_C(10))
            app->params.mass = MC_FP_C(10);
    }
    if (IsKeyDown(KEY_Z)) {
        app->params.mass -= MC_FP_C(0.5f);
        if (app->params.mass < MC_FP_C(0.1f))
            app->params.mass = MC_FP_C(0.1f);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        int dx = mx - MC_REAL_TO_INT(app->x);
        int dy = my - sh / 2;
        if (dx * dx + dy * dy < 1800) {
            app->grabbed = 1;
        }
        if (app->grabbed) {
            app->x = MC_REAL_FROM_INT(mx);
            app->v = 0;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (app->grabbed) {
            app->target_x = app->x;
            app->grabbed = 0;
        }
    }

    if (!app->grabbed) {
        mc_real_t click_target = MC_REAL_FROM_INT(mx);
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            app->target_x = click_target;
        }
        mc_spring_step(&app->x, (mc_spring_state_t*)&app->v,
            &app->params, app->target_x, MC_REAL_FROM_FLOAT(dt));
    }

    int cx = MC_REAL_TO_INT(app->x);
    int cy = sh / 2;

    ClearBackground(BLACK);
    DrawLine(0, cy, sw, cy, (Color){30, 30, 40, 255});
    DrawLine(MC_REAL_TO_INT(app->target_x), cy - 30,
             MC_REAL_TO_INT(app->target_x), cy + 30, GREEN);
    DrawCircle(cx, cy, 40, RED);
    DrawCircle(cx, cy, 25, (Color){255, 100, 100, 255});
    DrawCircle(cx, cy, 10, WHITE);
    DrawCircleLines(cx, cy, 40, (Color){255, 0, 0, 100});

    DrawText("Interactive Spring Physics", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Stiffness: %d  Damping: %d  Mass: %d",
        MC_REAL_TO_INT(app->params.stiffness),
        MC_REAL_TO_INT(app->params.damping),
        MC_REAL_TO_INT(app->params.mass)),
        10, 40, 14, LIGHTGRAY);
    DrawText(TextFormat("Position: %d  Velocity: %.2f  Target: %d",
        cx, MC_REAL_TO_FLOAT(app->v), MC_REAL_TO_INT(app->target_x)),
        10, 60, 14, LIGHTGRAY);

    if (app->show_help) {
        int hy = sh - 120;
        DrawRectangle(0, hy, sw, 120, (Color){0, 0, 0, 200});
        DrawText("Controls:", 10, hy + 5, 14, YELLOW);
        DrawText("  Up/Down    - Adjust stiffness", 10, hy + 25, 14, LIGHTGRAY);
        DrawText("  Left/Right  - Adjust damping", 10, hy + 45, 14, LIGHTGRAY);
        DrawText("  A/Z         - Adjust mass", 10, hy + 65, 14, LIGHTGRAY);
        DrawText("  Left drag   - Grab & pull mass", 10, hy + 85, 14, LIGHTGRAY);
        DrawText("  Right click - Set target", 10, hy + 105, 14, LIGHTGRAY);
        DrawText("  H           - Toggle help", 10, hy + 125, 14, LIGHTGRAY);
    }
}

int main(void) {
    app_t app;
    sut_example_run(800, 600, "Spring Physics - MotionC", on_setup, on_draw, &app);
    return 0;
}
