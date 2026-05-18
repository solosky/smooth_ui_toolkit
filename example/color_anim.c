#include <raylib.h>
#include "mc_animate.h"
#include "mc_color.h"
#include "raylib_helper.h"

typedef struct {
    mc_vec4_animate_t color_anim;
    Color current_color;
    Color colors[6];
    int color_index;
} app_t;

static void on_color_update(void* ctx, mc_vec4_t value) {
    app_t* app = (app_t*)ctx;
    app->current_color = (Color){
        (unsigned char)MC_REAL_TO_INT(mc_clamp(value.x, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(value.y, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(value.z, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(value.w, 0, MC_FP_C(255)))
    };
}

static mc_vec4_t color_to_vec4(Color c) {
    return mc_vec4(MC_REAL_FROM_INT(c.r), MC_REAL_FROM_INT(c.g),
                   MC_REAL_FROM_INT(c.b), MC_REAL_FROM_INT(c.a));
}

static void start_transition(app_t* app) {
    mc_vec4_t from = color_to_vec4(app->current_color);
    int next = (app->color_index + 1) % 6;
    mc_vec4_t to = color_to_vec4(app->colors[next]);

    mc_vec4_animate_init_easing(&app->color_anim, from, to, MC_FP_C(1.5f));
    app->color_anim.config.easing.easing = mc_ease_sine_in_out;
    app->color_anim.on_update = on_color_update;
    app->color_anim.ctx = app;
    app->color_index = next;
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->colors[0] = (Color){ 255, 0, 0, 255 };
    app->colors[1] = (Color){ 0, 255, 0, 255 };
    app->colors[2] = (Color){ 0, 0, 255, 255 };
    app->colors[3] = (Color){ 255, 255, 0, 255 };
    app->colors[4] = (Color){ 255, 0, 255, 255 };
    app->colors[5] = (Color){ 0, 255, 255, 255 };
    app->color_index = 0;
    app->current_color = app->colors[0];
    start_transition(app);
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();

    bool done = mc_vec4_animate_update(&app->color_anim, MC_REAL_FROM_FLOAT(dt));
    if (done) start_transition(app);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(app->current_color);

    int cx = sw / 2;
    int cy = sh / 2;
    int r = 80;
    Color complement = (Color){
        255 - app->current_color.r,
        255 - app->current_color.g,
        255 - app->current_color.b,
        255
    };

    DrawCircle(cx, cy, r, complement);
    DrawCircle(cx, cy, r - 10, app->current_color);

    DrawText("Color Animation", 10, 10, 20, complement);
    DrawText(TextFormat("RGB(%d, %d, %d)",
        app->current_color.r, app->current_color.g, app->current_color.b),
        10, 40, 14, complement);
    DrawText(TextFormat("Hex: #%02X%02X%02X",
        app->current_color.r, app->current_color.g, app->current_color.b),
        10, 60, 14, complement);

    mc_vec4_t cur = app->color_anim.current;
    DrawText(TextFormat("Vec4(%.0f, %.0f, %.0f, %.0f)",
        MC_REAL_TO_FLOAT(cur.x), MC_REAL_TO_FLOAT(cur.y),
        MC_REAL_TO_FLOAT(cur.z), MC_REAL_TO_FLOAT(cur.w)),
        10, 80, 12, complement);
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Color Animation - MotionC", on_setup, on_draw, &app);
    return 0;
}
