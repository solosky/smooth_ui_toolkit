#include <raylib.h>
#include <math.h>
#include "sut_animate.h"
#include "raylib_helper.h"

#define CURSOR_COUNT 6

typedef struct {
    sut_animate_t x;
    sut_animate_t y;
} cursor_t;

typedef struct {
    cursor_t cursors[CURSOR_COUNT];
    Color colors[CURSOR_COUNT];
} app_t;

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;

    unsigned int colors_raw[CURSOR_COUNT] = {
        0xFF0088FF, 0xDD00EEFF, 0x9911FFFF,
        0x7700FFFF, 0x4400FFFF, 0x0D63F8FF
    };

    for (int i = 0; i < CURSOR_COUNT; i++) {
        sut_animate_init_spring(&app->cursors[i].x, SUT_FP_C(400), SUT_FP_C(400));
        sut_animate_init_spring(&app->cursors[i].y, SUT_FP_C(225), SUT_FP_C(225));
        app->cursors[i].x.config.spring.stiffness = SUT_FP_C(55 + i * 25);
        app->cursors[i].x.config.spring.damping = SUT_FP_C(13 - i);
        app->cursors[i].y.config.spring = app->cursors[i].x.config.spring;
        app->colors[i].r = (colors_raw[i] >> 24) & 0xFF;
        app->colors[i].g = (colors_raw[i] >> 16) & 0xFF;
        app->colors[i].b = (colors_raw[i] >> 8) & 0xFF;
        app->colors[i].a = 255;
    }
    HideCursor();
}

static void update_cursors(app_t* app, int mx, int my) {
    float radius = 55.0f;
    float angle_step = 60.0f * (M_PI / 180.0f);
    for (int i = 0; i < CURSOR_COUNT; i++) {
        float angle = angle_step * i;
        int dx = (int)(radius * cosf(angle));
        int dy = (int)(radius * sinf(angle));
        app->cursors[i].x.to = SUT_REAL_FROM_INT(mx + dx);
        app->cursors[i].y.to = SUT_REAL_FROM_INT(my + dy);
    }
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int mx = GetMouseX();
    int my = GetMouseY();

    Rectangle win_rect = { 0, 0, 800, 450 };
    if (CheckCollisionPointRec((Vector2){ (float)mx, (float)my }, win_rect))
        update_cursors(app, mx, my);
    else
        update_cursors(app, 400, 225);

    for (int i = 0; i < CURSOR_COUNT; i++) {
        sut_animate_update(&app->cursors[i].x, SUT_REAL_FROM_FLOAT(dt));
        sut_animate_update(&app->cursors[i].y, SUT_REAL_FROM_FLOAT(dt));
    }

    DrawCircle(mx, my, 8, WHITE);
    for (int i = 0; i < CURSOR_COUNT; i++) {
        int cx = SUT_REAL_TO_INT(app->cursors[i].x.current);
        int cy = SUT_REAL_TO_INT(app->cursors[i].y.current);
        DrawCircle(cx, cy, 8, app->colors[i]);
    }
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Multi Cursor - C99", on_setup, on_draw, &app);
    return 0;
}
