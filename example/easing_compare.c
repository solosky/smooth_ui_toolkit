#include <raylib.h>
#include <string.h>
#include "mc_easing.h"
#include "raylib_helper.h"

#define EASE_COUNT 8

typedef struct {
    const char* name;
    mc_easing_fn_t fn;
    Color color;
    mc_real_t progress;
} ease_demo_t;

typedef struct {
    ease_demo_t eases[EASE_COUNT];
    float time;
    float speed;
} app_t;

static int rand_color_hex(void) {
    unsigned int colors[] = {
        0xFF4444, 0x44FF44, 0x4488FF, 0xFFFF44,
        0xFF44FF, 0x44FFFF, 0xFF8844, 0xFF4488
    };
    static int idx = 0;
    return colors[idx++ % 8];
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->eases[0] = (ease_demo_t){ "linear", mc_ease_linear, RED, 0 };
    app->eases[1] = (ease_demo_t){ "quad_out", mc_ease_quad_out, GREEN, 0 };
    app->eases[2] = (ease_demo_t){ "cubic_in_out", mc_ease_cubic_in_out, BLUE, 0 };
    app->eases[3] = (ease_demo_t){ "bounce_out", mc_ease_bounce_out, ORANGE, 0 };
    app->eases[4] = (ease_demo_t){ "elastic_out", mc_ease_elastic_out, MAGENTA, 0 };
    app->eases[5] = (ease_demo_t){ "back_out", mc_ease_back_out, SKYBLUE, 0 };
    app->eases[6] = (ease_demo_t){ "expo_out", mc_ease_expo_out, YELLOW, 0 };
    app->eases[7] = (ease_demo_t){ "circ_out", mc_ease_circ_out, LIME, 0 };
    app->time = 0;
    app->speed = 0.5f;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    if (IsKeyPressed(KEY_UP)) app->speed *= 1.5f;
    if (IsKeyPressed(KEY_DOWN)) app->speed *= 0.67f;
    if (app->speed < 0.05f) app->speed = 0.05f;
    if (app->speed > 5.0f) app->speed = 5.0f;

    app->time += dt * app->speed;
    mc_real_t t = MC_REAL_FROM_FLOAT(fmodf(app->time, 2.0f) / 2.0f);

    int margin = 60;
    int lane_h = (sh - 80) / EASE_COUNT;
    int ball_r = 12;

    ClearBackground(BLACK);

    for (int i = 0; i < EASE_COUNT; i++) {
        mc_real_t eased = app->eases[i].fn(t);
        app->eases[i].progress = eased;
        int lane_y = 30 + i * lane_h + lane_h / 2;

        int bar_x = margin;
        int bar_w = sw - margin * 2;
        int fill_w = (int)(MC_REAL_TO_FLOAT(eased) * bar_w);

        DrawRectangle(bar_x, lane_y - 8, bar_w, 16, (Color){20, 20, 30, 255});
        DrawRectangle(bar_x, lane_y - 8, fill_w, 16, (Color){
            app->eases[i].color.r / 3,
            app->eases[i].color.g / 3,
            app->eases[i].color.b / 3, 255
        });

        int bx = bar_x + fill_w;
        DrawCircle(bx, lane_y, ball_r, app->eases[i].color);
        DrawCircleLines(bx, lane_y, ball_r + 1, (Color){255, 255, 255, 60});

        DrawText(TextFormat("%s: %.2f", app->eases[i].name, MC_REAL_TO_FLOAT(eased)),
            10, lane_y - 6, 12, LIGHTGRAY);
    }

    int total_h = EASE_COUNT * lane_h;
    mc_real_t raw_t = MC_REAL_FROM_FLOAT(fmodf(app->time, 2.0f) / 2.0f);
    int cursor_x = margin + (int)(MC_REAL_TO_FLOAT(raw_t) * (sw - margin * 2));
    DrawLine(cursor_x, 10, cursor_x, 30 + total_h, (Color){255, 255, 255, 40});

    DrawText(TextFormat("Easing Comparison  (speed: %.1fx, UP/DOWN to adjust)", app->speed),
        10, 5, 12, RAYWHITE);
}

int main(void) {
    app_t app;
    sut_example_run(900, 600, "Easing Compare - MotionC", on_setup, on_draw, &app);
    return 0;
}
