#include <raylib.h>
#include <string.h>
#include "mc_easing.h"
#include "raylib_helper.h"

#define EASING_COUNT 6
#define CURVE_POINTS 200

typedef struct {
    const char* name;
    mc_easing_fn_t func;
} easing_entry_t;

typedef struct {
    easing_entry_t easings[EASING_COUNT];
    int selected;
    float time;
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->easings[0] = (easing_entry_t){ "ease_quad_out", mc_ease_quad_out };
    app->easings[1] = (easing_entry_t){ "ease_cubic_in_out", mc_ease_cubic_in_out };
    app->easings[2] = (easing_entry_t){ "ease_back_out", mc_ease_back_out };
    app->easings[3] = (easing_entry_t){ "ease_elastic_out", mc_ease_elastic_out };
    app->easings[4] = (easing_entry_t){ "ease_bounce_out", mc_ease_bounce_out };
    app->easings[5] = (easing_entry_t){ "ease_circ_in_out", mc_ease_circ_in_out };
    app->selected = 0;
    app->time = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN))
        app->selected = (app->selected + 1) % EASING_COUNT;
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP))
        app->selected = (app->selected - 1 + EASING_COUNT) % EASING_COUNT;

    app->time += dt;
    if (app->time > 2.0f) app->time = 0;

    mc_easing_fn_t fn = app->easings[app->selected].func;

    // Draw grid
    DrawLine(100, 350, 700, 350, DARKGRAY);
    DrawLine(100, 50, 100, 350, DARKGRAY);

    // Draw easing curve
    for (int i = 0; i < CURVE_POINTS; i++) {
        float t = (float)i / CURVE_POINTS;
        mc_real_t st = fn(MC_REAL_FROM_FLOAT(t));
        float eased = MC_REAL_TO_FLOAT(st);
        int x1 = 100 + (i * 600 / CURVE_POINTS);
        int y1 = 350 - (int)(eased * 300);
        if (i > 0) {
            int x0 = 100 + ((i - 1) * 600 / CURVE_POINTS);
            mc_real_t st0 = fn(MC_REAL_FROM_FLOAT((float)(i - 1) / CURVE_POINTS));
            float eased0 = MC_REAL_TO_FLOAT(st0);
            int y0 = 350 - (int)(eased0 * 300);
            DrawLine(x0, y0, x1, y1, LIME);
        }
    }

    // Draw animated ball on curve
    mc_real_t st = fn(MC_REAL_FROM_FLOAT(app->time / 2.0f));
    float eased = MC_REAL_TO_FLOAT(st);
    int bx = 100 + (int)((app->time / 2.0f) * 600);
    int by = 350 - (int)(eased * 300);
    DrawCircle(bx, by, 8, RED);
    DrawLine(bx, by, bx, 350, RED);

    DrawText(app->easings[app->selected].name, 10, 10, 20, RAYWHITE);
    DrawText("Arrow keys to switch easing", 10, 40, 16, DARKGRAY);
    DrawText(TextFormat("eased(%.2f) = %.2f", app->time / 2.0f, eased), 10, 65, 16, DARKGRAY);
    DrawFPS(700, 10);
}

int main(void) {
    app_t app;
    mc_example_run(800, 450, "Easing Curves - C99", on_setup, on_draw, &app);
    return 0;
}
