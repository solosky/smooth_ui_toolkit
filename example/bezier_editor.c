#include <raylib.h>
#include <string.h>
#include "mc_easing.h"
#include "raylib_helper.h"

typedef struct {
    mc_real_t x1, y1, x2, y2;
    int dragging;
    int hover_cp;
    float anim_t;
    mc_real_t sample_y;
} app_t;

static int hit_test(int mx, int my, int ox, int oy, int w, int h, mc_real_t x, mc_real_t y) {
    int px = ox + (int)(MC_REAL_TO_FLOAT(x) * w);
    int py = oy + h - (int)(MC_REAL_TO_FLOAT(y) * h);
    return (abs(mx - px) < 12 && abs(my - py) < 12);
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->x1 = MC_FP_C(0.25f);
    app->y1 = MC_FP_C(0.25f);
    app->x2 = MC_FP_C(0.75f);
    app->y2 = MC_FP_C(0.75f);
    app->dragging = -1;
    app->hover_cp = -1;
    app->anim_t = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int mx = GetMouseX();
    int my = GetMouseY();

    int gx = 50, gy = 50, gw = sw - 100, gh = sh - 120;

    app->anim_t += dt * 0.5f;
    if (app->anim_t > 1.0f) app->anim_t -= 1.0f;

    int cp1_x = gx + (int)(MC_REAL_TO_FLOAT(app->x1) * gw);
    int cp1_y = gy + gh - (int)(MC_REAL_TO_FLOAT(app->y1) * gh);
    int cp2_x = gx + (int)(MC_REAL_TO_FLOAT(app->x2) * gw);
    int cp2_y = gy + gh - (int)(MC_REAL_TO_FLOAT(app->y2) * gh);

    app->hover_cp = -1;
    if (hit_test(mx, my, gx, gy, gw, gh, app->x1, app->y1)) app->hover_cp = 0;
    if (hit_test(mx, my, gx, gy, gw, gh, app->x2, app->y2)) app->hover_cp = 1;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (app->hover_cp >= 0) app->dragging = app->hover_cp;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) app->dragging = -1;

    if (app->dragging >= 0 && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float rx = (float)(mx - gx) / gw;
        float ry = 1.0f - (float)(my - gy) / gh;
        rx = (rx < 0) ? 0 : (rx > 1) ? 1 : rx;
        ry = (ry < 0) ? 0 : (ry > 1) ? 1 : ry;
        if (app->dragging == 0) { app->x1 = MC_REAL_FROM_FLOAT(rx); app->y1 = MC_REAL_FROM_FLOAT(ry); }
        else { app->x2 = MC_REAL_FROM_FLOAT(rx); app->y2 = MC_REAL_FROM_FLOAT(ry); }
    }

    app->sample_y = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(app->anim_t), app->x1, app->y1, app->x2, app->y2);

    ClearBackground(BLACK);

    DrawRectangle(gx - 5, gy - 5, gw + 10, gh + 10, (Color){15, 15, 25, 255});
    DrawRectangleLines(gx - 5, gy - 5, gw + 10, gh + 10, (Color){40, 40, 60, 255});

    DrawLine(gx, gy + gh, cp1_x, cp1_y, (Color){100, 100, 150, 150});
    DrawLine(gx + gw, gy, cp2_x, cp2_y, (Color){100, 100, 150, 150});

    int pts = 100;
    for (int i = 1; i < pts; i++) {
        float t0 = (float)(i - 1) / pts;
        float t1 = (float)i / pts;
        mc_real_t bz0 = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(t0), app->x1, app->y1, app->x2, app->y2);
        mc_real_t bz1 = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(t1), app->x1, app->y1, app->x2, app->y2);
        int lx0 = gx + (int)(t0 * gw);
        int ly0 = gy + gh - (int)(MC_REAL_TO_FLOAT(bz0) * gh);
        int lx1 = gx + (int)(t1 * gw);
        int ly1 = gy + gh - (int)(MC_REAL_TO_FLOAT(bz1) * gh);
        DrawLine(lx0, ly0, lx1, ly1, SKYBLUE);
    }

    DrawCircle(cp1_x, cp1_y, 8, app->hover_cp == 0 ? YELLOW : ORANGE);
    DrawCircle(cp2_x, cp2_y, 8, app->hover_cp == 1 ? YELLOW : ORANGE);

    int bx = gx + (int)(app->anim_t * gw);
    int by = gy + gh - (int)(MC_REAL_TO_FLOAT(app->sample_y) * gh);
    DrawCircle(bx, by, 7, RED);
    DrawLine(bx, by, bx, gy + gh, (Color){255, 0, 0, 80});

    DrawText("Cubic Bezier Editor", 10, 10, 20, RAYWHITE);
    DrawText("Drag orange control points", 10, 35, 14, DARKGRAY);
    DrawText(TextFormat("CP1(%.2f, %.2f)  CP2(%.2f, %.2f)  t=%.2f  y=%.2f",
        MC_REAL_TO_FLOAT(app->x1), MC_REAL_TO_FLOAT(app->y1),
        MC_REAL_TO_FLOAT(app->x2), MC_REAL_TO_FLOAT(app->y2),
        app->anim_t, MC_REAL_TO_FLOAT(app->sample_y)),
        10, sh - 25, 14, LIGHTGRAY);

    DrawText("sut_ease_cubic_bezier", 10, 55, 12, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(900, 600, "Bezier Editor - MotionC", on_setup, on_draw, &app);
    return 0;
}
