#include <raylib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mc_animate.h"
#include "mc_easing.h"
#include "mc_spring.h"
#include "mc_sequence.h"
#include "mc_color.h"
#include "mc_math.h"
#include "raylib_helper.h"

#define DEMO_COUNT 8
#define MAX_BALLS  6
#define EASING_COLS 4
#define EASING_TOTAL 31

static const char* demo_titles[DEMO_COUNT] = {
    "Easing Gallery - All 31 Easing Functions",
    "Scalar Easing Animations - Duration-Based",
    "Scalar Spring Animations - Physics-Based",
    "Vec2 Animations - 2D Positional",
    "Vec4 Animations - Color Transitions",
    "Animation Sequences - Chained Steps",
    "Raw Spring Physics - Interactive",
    "Math & Color Utilities + Cubic Bezier"
};

typedef struct {
    const char* name;
    mc_easing_fn_t fn;
} easing_entry_t;

static const easing_entry_t easing_list[EASING_TOTAL] = {
    {"linear",       mc_ease_linear},
    {"quad_in",      mc_ease_quad_in},
    {"quad_out",     mc_ease_quad_out},
    {"quad_in_out",  mc_ease_quad_in_out},
    {"cubic_in",     mc_ease_cubic_in},
    {"cubic_out",    mc_ease_cubic_out},
    {"cubic_in_out", mc_ease_cubic_in_out},
    {"quart_in",     mc_ease_quart_in},
    {"quart_out",    mc_ease_quart_out},
    {"quart_in_out", mc_ease_quart_in_out},
    {"quint_in",     mc_ease_quint_in},
    {"quint_out",    mc_ease_quint_out},
    {"quint_in_out", mc_ease_quint_in_out},
    {"sine_in",      mc_ease_sine_in},
    {"sine_out",     mc_ease_sine_out},
    {"sine_in_out",  mc_ease_sine_in_out},
    {"expo_in",      mc_ease_expo_in},
    {"expo_out",     mc_ease_expo_out},
    {"expo_in_out",  mc_ease_expo_in_out},
    {"circ_in",      mc_ease_circ_in},
    {"circ_out",     mc_ease_circ_out},
    {"circ_in_out",  mc_ease_circ_in_out},
    {"back_in",      mc_ease_back_in},
    {"back_out",     mc_ease_back_out},
    {"back_in_out",  mc_ease_back_in_out},
    {"elastic_in",   mc_ease_elastic_in},
    {"elastic_out",  mc_ease_elastic_out},
    {"elastic_in_out", mc_ease_elastic_in_out},
    {"bounce_in",    mc_ease_bounce_in},
    {"bounce_out",   mc_ease_bounce_out},
    {"bounce_in_out", mc_ease_bounce_in_out},
};

typedef struct {
    mc_animate_t anim;
    Color color;
    int idx;
    char label[32];
} ball_t;

typedef struct {
    mc_vec4_animate_t anim;
    Color color;
    float phase;
} color_ball_t;

typedef struct {
    mc_sequence_t seq;
    float time;
    int step_index;
} seq_demo_t;

typedef struct {
    mc_real_t x, v;
    mc_spring_params_t params;
    mc_real_t target;
    int target_x;
} spring_raw_t;

typedef struct {
    mc_real_t t;
    mc_real_t x1, y1, x2, y2;
    int selected_cp;
} bezier_demo_t;

typedef struct {
    int current_demo;
    float global_time;

    ball_t balls[MAX_BALLS];
    int ball_count;

    mc_vec2_animate_t vec2_ease;
    mc_vec2_animate_t vec2_spring;

    color_ball_t col_balls[4];

    seq_demo_t seq_demo;

    spring_raw_t spring_demo;

    bezier_demo_t bezier;

    mc_animate_t spring_balls[MAX_BALLS];
    Color spring_ball_colors[MAX_BALLS];
    float spring_phases[MAX_BALLS];

    mc_animate_t bounce_anim;
    mc_animate_t scale_anim;
} app_t;

static Color hsv_to_color(float h, float s, float v) {
    mc_rgb_t srgb = mc_rgb_from_hsv(MC_REAL_FROM_FLOAT(h), MC_REAL_FROM_FLOAT(s), MC_REAL_FROM_FLOAT(v));
    return (Color){srgb.r, srgb.g, srgb.b, 255};
}

static Color rgba_to_color(mc_vec4_t v) {
    return (Color){
        (unsigned char)MC_REAL_TO_INT(mc_clamp(v.x, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(v.y, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(v.z, 0, MC_FP_C(255))),
        (unsigned char)MC_REAL_TO_INT(mc_clamp(v.w, 0, MC_FP_C(255)))
    };
}

static void on_ball_update(void* ctx, mc_real_t value) {
    (void)value;
    ball_t* b = (ball_t*)ctx;
    b->idx++;
}

static void setup_easing_gallery(app_t* app) {
    (void)app;
}

static void draw_easing_gallery(app_t* app) {
    int rows = (EASING_TOTAL + EASING_COLS - 1) / EASING_COLS;
    int gw = GetScreenWidth() / EASING_COLS;
    int gh = (GetScreenHeight() - 30) / rows;
    int pad = 6;
    int curve_pts = 60;
    int label_h = 14;

    for (int i = 0; i < EASING_TOTAL; i++) {
        int col = i % EASING_COLS;
        int row = i / EASING_COLS;
        int ox = col * gw + pad;
        int oy = row * gh + pad;
        int cw = gw - pad * 2;
        int ch = gh - pad * 2 - label_h - 2;

        DrawRectangle(ox, oy, cw, ch + label_h + 2, (Color){20, 20, 30, 255});
        DrawRectangleLines(ox, oy, cw, ch + label_h + 2, (Color){50, 50, 70, 255});

        int curve_bot = oy + ch;
        int curve_left = ox;

        mc_easing_fn_t fn = easing_list[i].fn;

        for (int p = 0; p < curve_pts; p++) {
            float t0 = (float)p / curve_pts;
            float t1 = (float)(p + 1) / curve_pts;
            mc_real_t st0 = fn(MC_REAL_FROM_FLOAT(t0));
            mc_real_t st1 = fn(MC_REAL_FROM_FLOAT(t1));
            int x0 = curve_left + (int)(t0 * cw);
            int y0 = curve_bot - (int)(MC_REAL_TO_FLOAT(st0) * ch);
            int x1 = curve_left + (int)(t1 * cw);
            int y1 = curve_bot - (int)(MC_REAL_TO_FLOAT(st1) * ch);
            DrawLine(x0, y0, x1, y1, LIME);
        }

        float anim_t = fmodf(app->global_time, 2.0f) / 2.0f;
        mc_real_t sv = fn(MC_REAL_FROM_FLOAT(anim_t));
        float val = MC_REAL_TO_FLOAT(sv);
        int bx = curve_left + (int)(anim_t * cw);
        int by = curve_bot - (int)(val * ch);
        DrawCircle(bx, by, 4, RED);
        DrawLine(bx, by, bx, curve_bot, (Color){255, 0, 0, 80});

        int label_y = oy + ch + 2;
        DrawText(easing_list[i].name, ox + 2, label_y, 12, RAYWHITE);
    }
}

static void setup_scalar_easing(app_t* app) {
    mc_real_t w = MC_REAL_FROM_FLOAT(GetScreenWidth() - 100);
    app->ball_count = 4;
    mc_real_t durs[] = {MC_FP_C(0.5), MC_FP_C(1.0), MC_FP_C(1.5), MC_FP_C(2.0)};
    mc_easing_fn_t eases[] = {mc_ease_quad_out, mc_ease_elastic_out, mc_ease_bounce_out, mc_ease_cubic_in_out};
    Color colors[] = {RED, GREEN, BLUE, ORANGE};
    const char* labels[] = {"quad_out 0.5s", "elastic_out 1.0s", "bounce_out 1.5s", "cubic_in_out 2.0s"};

    for (int i = 0; i < app->ball_count; i++) {
        mc_animate_init_easing(&app->balls[i].anim, MC_FP_C(50), w, durs[i]);
        app->balls[i].anim.repeat = 255;
        app->balls[i].anim.config.easing.easing = eases[i];
        app->balls[i].color = colors[i];
        app->balls[i].idx = 0;
        strncpy(app->balls[i].label, labels[i], sizeof(app->balls[i].label) - 1);
        app->balls[i].label[sizeof(app->balls[i].label) - 1] = '\0';

        app->balls[i].anim.on_update = on_ball_update;
        app->balls[i].anim.ctx = &app->balls[i];
    }
}

static void draw_scalar_easing(app_t* app) {
    float dt = GetFrameTime();
    int base_y = 80;

    for (int i = 0; i < app->ball_count; i++) {
        mc_animate_update(&app->balls[i].anim, MC_REAL_FROM_FLOAT(dt));
        int x = MC_REAL_TO_INT(app->balls[i].anim.current);
        int y = base_y + i * 90;
        DrawCircle(x, y, 25, app->balls[i].color);
        DrawText(app->balls[i].label, 10, y - 30, 14, LIGHTGRAY);
        DrawText(TextFormat("x=%d", x), 10, y + 30, 12, DARKGRAY);
    }

    DrawText("All easing animations repeat=255 (infinite loop)", 10, 10, 16, RAYWHITE);
}

static void setup_scalar_spring(app_t* app) {
    for (int i = 0; i < MAX_BALLS; i++) {
        mc_animate_init_spring(&app->spring_balls[i],
            MC_FP_C(100 + i * 60), MC_FP_C(100 + i * 60));
        app->spring_balls[i].config.spring.stiffness = MC_FP_C(50 + i * 30);
        app->spring_balls[i].config.spring.damping = MC_FP_C(12 - i);
        if (app->spring_balls[i].config.spring.damping < MC_FP_C(3))
            app->spring_balls[i].config.spring.damping = MC_FP_C(3);
        app->spring_balls[i].repeat = 255;
        app->spring_ball_colors[i] = mc_example_random_color();
        app->spring_phases[i] = (float)i / MAX_BALLS;
    }
}

static void draw_scalar_spring(app_t* app) {
    float dt = GetFrameTime();
    int mx = GetMouseX();
    int my = GetMouseY();
    int base_y = 80;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < MAX_BALLS; i++) {
            int offset = (i - MAX_BALLS / 2) * 40;
            app->spring_balls[i].from = app->spring_balls[i].current;
            app->spring_balls[i].to = MC_REAL_FROM_FLOAT(mx + offset);
            app->spring_balls[i].elapsed = 0;
            app->spring_balls[i].velocity = 0;
        }
    }

    for (int i = 0; i < MAX_BALLS; i++) {
        mc_animate_update(&app->spring_balls[i], MC_REAL_FROM_FLOAT(dt));
        int x = MC_REAL_TO_INT(app->spring_balls[i].current);
        int y = base_y + i * 90;
        DrawCircle(x, y, 20, app->spring_ball_colors[i]);
        DrawText(TextFormat("k=%d d=%d",
            MC_REAL_TO_INT(app->spring_balls[i].config.spring.stiffness),
            MC_REAL_TO_INT(app->spring_balls[i].config.spring.damping)),
            x + 25, y - 8, 12, LIGHTGRAY);
    }

    DrawText("Click to retarget all springs to mouse X", 10, 10, 16, RAYWHITE);
    DrawText("Each ball has different stiffness/damping", 10, 30, 14, DARKGRAY);
    (void)my;
}

static void setup_vec2_animations(app_t* app) {
    mc_vec2_t from = mc_vec2(MC_FP_C(100), MC_FP_C(200));
    mc_vec2_t to = mc_vec2(MC_FP_C(500), MC_FP_C(200));
    mc_vec2_animate_init_easing(&app->vec2_ease, from, to, MC_FP_C(1.5));
    app->vec2_ease.config.easing.easing = mc_ease_cubic_in_out;
    app->vec2_ease.ctx = NULL;

    mc_vec2_t sfrom = mc_vec2(MC_FP_C(100), MC_FP_C(400));
    mc_vec2_t sto = mc_vec2(MC_FP_C(500), MC_FP_C(400));
    mc_vec2_animate_init_spring(&app->vec2_spring, sfrom, sto);
    app->vec2_spring.config.spring.stiffness = MC_FP_C(120);
    app->vec2_spring.config.spring.damping = MC_FP_C(10);
}

static void draw_vec2_animations(app_t* app) {
    float dt = GetFrameTime();
    int mx = GetMouseX();
    int my = GetMouseY();

    mc_vec2_animate_update(&app->vec2_ease, MC_REAL_FROM_FLOAT(dt));
    mc_vec2_animate_update(&app->vec2_spring, MC_REAL_FROM_FLOAT(dt));

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        mc_vec2_t target = mc_vec2(MC_REAL_FROM_FLOAT(mx), MC_REAL_FROM_FLOAT(my));
        mc_vec2_animate_move_to(&app->vec2_ease, target);
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        mc_vec2_t target = mc_vec2(MC_REAL_FROM_FLOAT(mx), MC_REAL_FROM_FLOAT(my));
        mc_vec2_animate_move_to(&app->vec2_spring, target);
    }

    int ex = MC_REAL_TO_INT(app->vec2_ease.current.x);
    int ey = MC_REAL_TO_INT(app->vec2_ease.current.y);
    DrawCircle(ex, ey, 25, BLUE);
    DrawText("Easing (Left-click)", ex + 30, ey - 10, 14, LIGHTGRAY);

    int sx = MC_REAL_TO_INT(app->vec2_spring.current.x);
    int sy = MC_REAL_TO_INT(app->vec2_spring.current.y);
    DrawCircle(sx, sy, 25, YELLOW);
    DrawText("Spring (Right-click)", sx + 30, sy - 10, 14, LIGHTGRAY);

    DrawText("Vec2 Animation Demo", 10, 10, 16, RAYWHITE);
}

static void on_color_update(void* ctx, mc_vec4_t value) {
    (void)value;
    color_ball_t* cb = (color_ball_t*)ctx;
    cb->color = rgba_to_color(value);
}

static void setup_vec4_animations(app_t* app) {
    mc_vec4_t colors[4] = {
        mc_vec4(MC_FP_C(255), MC_FP_C(0), MC_FP_C(0), MC_FP_C(255)),
        mc_vec4(MC_FP_C(0), MC_FP_C(255), MC_FP_C(0), MC_FP_C(255)),
        mc_vec4(MC_FP_C(0), MC_FP_C(0), MC_FP_C(255), MC_FP_C(255)),
        mc_vec4(MC_FP_C(255), MC_FP_C(255), MC_FP_C(0), MC_FP_C(255)),
    };
    mc_vec4_t targets[4] = {
        mc_vec4(MC_FP_C(0), MC_FP_C(255), MC_FP_C(255), MC_FP_C(255)),
        mc_vec4(MC_FP_C(255), MC_FP_C(0), MC_FP_C(255), MC_FP_C(255)),
        mc_vec4(MC_FP_C(255), MC_FP_C(255), MC_FP_C(0), MC_FP_C(128)),
        mc_vec4(MC_FP_C(0), MC_FP_C(128), MC_FP_C(255), MC_FP_C(255)),
    };

    for (int i = 0; i < 4; i++) {
        mc_vec4_animate_init_easing(&app->col_balls[i].anim, colors[i], targets[i], MC_FP_C(1.5 + i * 0.5));
        app->col_balls[i].anim.config.easing.easing = mc_ease_sine_in_out;
        app->col_balls[i].anim.on_update = on_color_update;
        app->col_balls[i].anim.ctx = &app->col_balls[i];
        app->col_balls[i].color = rgba_to_color(colors[i]);
        app->col_balls[i].phase = (float)i * 0.25f;
    }
}

static void draw_vec4_animations(app_t* app) {
    float dt = GetFrameTime();

    for (int i = 0; i < 4; i++) {
        bool done = mc_vec4_animate_update(&app->col_balls[i].anim, MC_REAL_FROM_FLOAT(dt));
        if (done) {
            mc_vec4_t tmp = app->col_balls[i].anim.from;
            app->col_balls[i].anim.from = app->col_balls[i].anim.to;
            app->col_balls[i].anim.to = tmp;
            app->col_balls[i].anim.elapsed = 0;
        }
        int x = 150 + i * 250;
        int y = 200;
        DrawCircle(x, y, 60, app->col_balls[i].color);

        mc_vec4_t c = app->col_balls[i].anim.current;
        DrawText(TextFormat("RGBA(%d,%d,%d,%d)",
            MC_REAL_TO_INT(c.x), MC_REAL_TO_INT(c.y),
            MC_REAL_TO_INT(c.z), MC_REAL_TO_INT(c.w)),
            x - 60, y + 70, 14, LIGHTGRAY);
    }

    DrawText("Vec4 Color Animation (sine_in_out, repeat=255)", 10, 10, 16, RAYWHITE);
}

static void on_seq_complete(void* ctx) {
    (void)ctx;
}

static void on_seq_step(void* ctx, int index) {
    app_t* app = (app_t*)ctx;
    app->seq_demo.step_index = index;
}

static void setup_sequence(app_t* app) {
    mc_sequence_init(&app->seq_demo.seq, 8);
    app->seq_demo.seq.on_step = on_seq_step;
    app->seq_demo.seq.on_complete = on_seq_complete;
    app->seq_demo.seq.ctx = app;

    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(100), MC_FP_C(0.5), mc_ease_quad_out});
    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(300), MC_FP_C(0.8), mc_ease_bounce_out});
    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(500), MC_FP_C(0.6), mc_ease_elastic_out});
    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(200), MC_FP_C(0.4), mc_ease_cubic_in_out});
    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(600), MC_FP_C(0.7), mc_ease_back_out});
    mc_sequence_push(&app->seq_demo.seq,
        (mc_animate_step_t){MC_FP_C(50),  MC_FP_C(0.5), mc_ease_quad_in});

    mc_sequence_play(&app->seq_demo.seq);
    app->seq_demo.step_index = 0;
}

static void draw_sequence(app_t* app) {
    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_SPACE)) {
        mc_sequence_play(&app->seq_demo.seq);
        app->seq_demo.step_index = 0;
    }

    bool done = mc_sequence_update(&app->seq_demo.seq, MC_REAL_FROM_FLOAT(dt));

    int bar_x = 100;
    int bar_y = 350;
    int bar_w = 600;
    int bar_h = 30;

    DrawRectangle(bar_x, bar_y, bar_w, bar_h, (Color){30, 30, 40, 255});
    DrawRectangleLines(bar_x, bar_y, bar_w, bar_h, (Color){60, 60, 80, 255});

    if (app->seq_demo.seq.count > 0) {
        mc_animate_step_t* cur = &app->seq_demo.seq.steps[app->seq_demo.step_index];
        float progress = (float)app->seq_demo.seq.count > 0 ?
            MC_REAL_TO_FLOAT(mc_lerp(0, MC_FP_C(1),
                MC_REAL_FROM_FLOAT(app->seq_demo.seq.elapsed / MC_REAL_TO_FLOAT(cur->duration)))) : 0;
        if (progress > 1.0f) progress = 1.0f;
        int fill_w = (int)(progress * bar_w);
        DrawRectangle(bar_x + 1, bar_y + 1, fill_w - 2, bar_h - 2, (Color){0, 180, 255, 200});
    }

    if (done && app->seq_demo.seq.count > 0) {
        DrawRectangle(bar_x + 1, bar_y + 1, bar_w - 2, bar_h - 2, GREEN);
    }

    for (unsigned int i = 0; i < app->seq_demo.seq.count; i++) {
        int tx = 100 + (int)i * 80;
        int ty = 200;
        Color c = (i == app->seq_demo.step_index) ? GREEN : DARKGRAY;
        if (i < app->seq_demo.step_index) c = (Color){0, 180, 80, 255};
        DrawCircle(tx, ty, 6, c);
        DrawText(TextFormat("Step %d", i + 1), tx - 10, ty + 12, 10, LIGHTGRAY);
        DrawText(TextFormat("t=%.1fs", MC_REAL_TO_FLOAT(app->seq_demo.seq.steps[i].duration)),
            tx - 12, ty + 24, 9, DARKGRAY);
    }

    DrawText("Animation Sequence", 10, 10, 16, RAYWHITE);
    DrawText("Press SPACE to replay", 10, 35, 14, DARKGRAY);
    DrawText(TextFormat("Step: %d / %d  %s",
        app->seq_demo.step_index + 1, app->seq_demo.seq.count,
        done ? "COMPLETE" : "running..."),
        10, 55, 14, LIGHTGRAY);
}

static void setup_spring_raw(app_t* app) {
    app->spring_demo.x = MC_REAL_FROM_FLOAT(GetScreenWidth() / 2);
    app->spring_demo.v = 0;
    app->spring_demo.params.stiffness = MC_FP_C(80);
    app->spring_demo.params.damping = MC_FP_C(8);
    app->spring_demo.params.mass = MC_FP_C(1);
    app->spring_demo.target = app->spring_demo.x;
    app->spring_demo.target_x = GetScreenWidth() / 2;
}

static void draw_spring_raw(app_t* app) {
    float dt = GetFrameTime();
    int mx = GetMouseX();
    int my = GetMouseY();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        app->spring_demo.target = MC_REAL_FROM_FLOAT(mx);
        app->spring_demo.target_x = mx;
    }

    mc_spring_step(&app->spring_demo.x, (mc_spring_state_t*)&app->spring_demo.v,
        &app->spring_demo.params, app->spring_demo.target, MC_REAL_FROM_FLOAT(dt));

    int cx = MC_REAL_TO_INT(app->spring_demo.x);
    int cy = sh / 2;

    DrawCircle(cx, cy, 40, RED);
    DrawCircle(cx, cy, 25, (Color){255, 100, 100, 255});
    DrawCircle(cx, cy, 10, WHITE);

    DrawLine(cx, cy - 50, cx, cy + 50, (Color){255, 0, 0, 60});
    DrawLine(app->spring_demo.target_x, cy - 60, app->spring_demo.target_x, cy + 60, (Color){0, 255, 0, 100});

    int params_y = 20;
    DrawText("Raw Spring Physics (mc_spring_step)", 10, 10, 16, RAYWHITE);
    DrawText("Click to set target", 10, 35, 14, DARKGRAY);

    DrawText(TextFormat("Stiffness: %d", MC_REAL_TO_INT(app->spring_demo.params.stiffness)),
        10, params_y + 60, 14, LIGHTGRAY);
    DrawText(TextFormat("Damping: %d", MC_REAL_TO_INT(app->spring_demo.params.damping)),
        10, params_y + 80, 14, LIGHTGRAY);
    DrawText(TextFormat("Mass: %d", MC_REAL_TO_INT(app->spring_demo.params.mass)),
        10, params_y + 100, 14, LIGHTGRAY);
    DrawText(TextFormat("Velocity: %.2f", MC_REAL_TO_FLOAT(app->spring_demo.v)),
        10, params_y + 120, 14, LIGHTGRAY);

    if (IsKeyDown(KEY_UP)) {
        app->spring_demo.params.stiffness += MC_FP_C(2);
        if (app->spring_demo.params.stiffness > MC_FP_C(500))
            app->spring_demo.params.stiffness = MC_FP_C(500);
    }
    if (IsKeyDown(KEY_DOWN)) {
        app->spring_demo.params.stiffness -= MC_FP_C(2);
        if (app->spring_demo.params.stiffness < MC_FP_C(1))
            app->spring_demo.params.stiffness = MC_FP_C(1);
    }
    if (IsKeyDown(KEY_LEFT)) {
        app->spring_demo.params.damping -= MC_FP_C(1);
        if (app->spring_demo.params.damping < MC_FP_C(1))
            app->spring_demo.params.damping = MC_FP_C(1);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        app->spring_demo.params.damping += MC_FP_C(1);
        if (app->spring_demo.params.damping > MC_FP_C(50))
            app->spring_demo.params.damping = MC_FP_C(50);
    }

    (void)my;
    (void)sw;
}

static void setup_math_color(app_t* app) {
    app->bezier.t = 0;
    app->bezier.x1 = MC_FP_C(0.25);
    app->bezier.y1 = MC_FP_C(0.1);
    app->bezier.x2 = MC_FP_C(0.75);
    app->bezier.y2 = MC_FP_C(0.9);
    app->bezier.selected_cp = -1;

    mc_animate_init_easing(&app->bounce_anim, MC_FP_C(0), MC_FP_C(360), MC_FP_C(3));
    app->bounce_anim.repeat = 255;
    app->bounce_anim.config.easing.easing = mc_ease_bounce_out;

    mc_animate_init_easing(&app->scale_anim, MC_FP_C(20), MC_FP_C(60), MC_FP_C(1.5));
    app->scale_anim.repeat = 255;
    app->scale_anim.config.easing.easing = mc_ease_elastic_out;
}

static void draw_math_color(app_t* app) {
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int mx = GetMouseX();
    int my = GetMouseY();

    mc_animate_update(&app->bounce_anim, MC_REAL_FROM_FLOAT(dt));
    mc_animate_update(&app->scale_anim, MC_REAL_FROM_FLOAT(dt));

    int cx = MC_REAL_TO_INT(mc_lerp(MC_FP_C(100), MC_FP_C(700),
        MC_REAL_FROM_FLOAT(fmodf(app->global_time * 0.3f, 1.0f))));
    int cy = MC_REAL_TO_INT(mc_map(
        MC_REAL_FROM_FLOAT(sinf(app->global_time * 2.0f)),
        MC_FP_C(-1), MC_FP_C(1), MC_FP_C(100), MC_FP_C(400)));
    DrawCircle(cx, cy, 15, hsv_to_color(fmodf(app->global_time * 50, 360), 1, 1));
    DrawText("mc_lerp / mc_map / hsv", cx - 40, cy + 20, 12, LIGHTGRAY);

    mc_rgb_t c1 = mc_rgb_from_hex(0xFF5733);
    mc_rgb_t c2 = mc_rgb_from_hex(0x33FF57);
    mc_rgb_t blended = mc_rgb_blend_opacity(c1, c2,
        MC_REAL_FROM_FLOAT(fmodf(app->global_time * 0.5f, 1.0f)));
    mc_rgb_t diff = mc_rgb_blend_difference(c1, c2);

    int by = 80;
    DrawRectangle(100, by, 80, 40, (Color){c1.r, c1.g, c1.b, 255});
    DrawRectangle(200, by, 80, 40, (Color){c2.r, c2.g, c2.b, 255});
    DrawRectangle(300, by, 80, 40, (Color){blended.r, blended.g, blended.b, 255});
    DrawRectangle(400, by, 80, 40, (Color){diff.r, diff.g, diff.b, 255});
    DrawText("hex->blend_opacity->diff", 100, by + 45, 12, LIGHTGRAY);

    mc_rgb_t from_hsv = mc_rgb_from_hsv(
        MC_REAL_FROM_FLOAT(fmodf(app->global_time * 60, 360)),
        MC_FP_C(1), MC_FP_C(1));
    DrawRectangle(550, by, 80, 40, (Color){from_hsv.r, from_hsv.g, from_hsv.b, 255});
    DrawText("hsv->rgb", 570, by + 45, 12, LIGHTGRAY);

    int bz_cx = 600;
    int bz_cy = 500;
    int bz_w = 500;
    int bz_h = 200;
    int bz_ox = bz_cx - bz_w / 2;
    int bz_oy = bz_cy - bz_h / 2;

    DrawRectangle(bz_ox - 10, bz_oy - 10, bz_w + 20, bz_h + 20, (Color){15, 15, 25, 255});
    DrawRectangleLines(bz_ox - 10, bz_oy - 10, bz_w + 20, bz_h + 20, (Color){40, 40, 60, 255});

    mc_real_t x1_p = app->bezier.x1;
    mc_real_t y1_p = app->bezier.y1;
    mc_real_t x2_p = app->bezier.x2;
    mc_real_t y2_p = app->bezier.y2;

    int cp1_x = bz_ox + MC_REAL_TO_INT(mc_clamp(x1_p, 0, MC_FP_C(1)) * bz_w);
    int cp1_y = bz_oy + bz_h - MC_REAL_TO_INT(mc_clamp(y1_p, 0, MC_FP_C(1)) * bz_h);
    int cp2_x = bz_ox + MC_REAL_TO_INT(mc_clamp(x2_p, 0, MC_FP_C(1)) * bz_w);
    int cp2_y = bz_oy + bz_h - MC_REAL_TO_INT(mc_clamp(y2_p, 0, MC_FP_C(1)) * bz_h);

    DrawLine(bz_ox, bz_oy + bz_h, cp1_x, cp1_y, (Color){100, 100, 150, 255});
    DrawLine(bz_ox + bz_w, bz_oy, cp2_x, cp2_y, (Color){100, 100, 150, 255});

    for (int p = 0; p < 100; p++) {
        float t0 = (float)p / 100;
        float t1 = (float)(p + 1) / 100;
        mc_real_t bz0 = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(t0), x1_p, y1_p, x2_p, y2_p);
        mc_real_t bz1 = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(t1), x1_p, y1_p, x2_p, y2_p);
        int lx0 = bz_ox + (int)(t0 * bz_w);
        int ly0 = bz_oy + bz_h - MC_REAL_TO_INT(mc_clamp(bz0, 0, MC_FP_C(1)) * bz_h);
        int lx1 = bz_ox + (int)(t1 * bz_w);
        int ly1 = bz_oy + bz_h - MC_REAL_TO_INT(mc_clamp(bz1, 0, MC_FP_C(1)) * bz_h);
        DrawLine(lx0, ly0, lx1, ly1, SKYBLUE);
    }

    DrawCircle(cp1_x, cp1_y, 8, ORANGE);
    DrawCircle(cp2_x, cp2_y, 8, ORANGE);

    float bz_anim_t = fmodf(app->global_time, 2.0f) / 2.0f;
    mc_real_t bz_val = mc_ease_cubic_bezier(MC_REAL_FROM_FLOAT(bz_anim_t), x1_p, y1_p, x2_p, y2_p);
    int bb_x = bz_ox + (int)(bz_anim_t * bz_w);
    int bb_y = bz_oy + bz_h - MC_REAL_TO_INT(mc_clamp(bz_val, 0, MC_FP_C(1)) * bz_h);
    DrawCircle(bb_x, bb_y, 7, RED);
    DrawLine(bb_x, bb_y, bb_x, bz_oy + bz_h, (Color){255, 0, 0, 80});

    DrawText("Cubic Bezier (mc_ease_cubic_bezier)", bz_ox, bz_oy - 20, 14, RAYWHITE);
    DrawText(TextFormat("CP1(%.2f,%.2f) CP2(%.2f,%.2f)",
        MC_REAL_TO_FLOAT(x1_p), MC_REAL_TO_FLOAT(y1_p),
        MC_REAL_TO_FLOAT(x2_p), MC_REAL_TO_FLOAT(y2_p)),
        bz_ox, bz_oy + bz_h + 5, 12, LIGHTGRAY);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float rx = (float)(mx - bz_ox) / bz_w;
        float ry = 1.0f - (float)(my - bz_oy) / bz_h;
        if (rx >= 0 && rx <= 1 && ry >= 0 && ry <= 1) {
            int d1 = abs(mx - cp1_x) + abs(my - cp1_y);
            int d2 = abs(mx - cp2_x) + abs(my - cp2_y);
            if (d1 < d2 && d1 < 30) {
                app->bezier.x1 = MC_REAL_FROM_FLOAT(rx);
                app->bezier.y1 = MC_REAL_FROM_FLOAT(ry);
            } else if (d2 < 30) {
                app->bezier.x2 = MC_REAL_FROM_FLOAT(rx);
                app->bezier.y2 = MC_REAL_FROM_FLOAT(ry);
            }
        }
    }

    (void)my;
    (void)sw;
    (void)sh;
}

typedef void (*setup_fn_t)(app_t*);
typedef void (*draw_fn_t)(app_t*);

static const setup_fn_t setup_fns[DEMO_COUNT] = {
    setup_easing_gallery,
    setup_scalar_easing,
    setup_scalar_spring,
    setup_vec2_animations,
    setup_vec4_animations,
    setup_sequence,
    setup_spring_raw,
    setup_math_color
};

static const draw_fn_t draw_fns[DEMO_COUNT] = {
    draw_easing_gallery,
    draw_scalar_easing,
    draw_scalar_spring,
    draw_vec2_animations,
    draw_vec4_animations,
    draw_sequence,
    draw_spring_raw,
    draw_math_color
};

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    memset(app, 0, sizeof(app_t));
    app->current_demo = 0;

    for (int i = 0; i < DEMO_COUNT; i++) {
        if (setup_fns[i]) setup_fns[i](app);
    }
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    app->global_time += dt;

    if (IsKeyPressed(KEY_ONE))   app->current_demo = 0;
    if (IsKeyPressed(KEY_TWO))   app->current_demo = 1;
    if (IsKeyPressed(KEY_THREE)) app->current_demo = 2;
    if (IsKeyPressed(KEY_FOUR))  app->current_demo = 3;
    if (IsKeyPressed(KEY_FIVE))  app->current_demo = 4;
    if (IsKeyPressed(KEY_SIX))   app->current_demo = 5;
    if (IsKeyPressed(KEY_SEVEN)) app->current_demo = 6;
    if (IsKeyPressed(KEY_EIGHT)) app->current_demo = 7;

    int demo = app->current_demo;

    if (demo >= 0 && demo < DEMO_COUNT && draw_fns[demo]) {
        draw_fns[demo](app);
    }

    DrawRectangle(0, GetScreenHeight() - 30, GetScreenWidth(), 30, (Color){10, 10, 20, 200});
    char header[256];
    snprintf(header, sizeof(header), "[%d/7] %s  |  1-8: Switch Demo",
        demo, demo_titles[demo]);
    DrawText(header, 10, GetScreenHeight() - 25, 14, (Color){200, 200, 200, 255});
    DrawFPS(GetScreenWidth() - 80, GetScreenHeight() - 25);

    (void)dt;
}

int main(void) {
    app_t app;
    memset(&app, 0, sizeof(app));
    mc_example_run(1200, 800, "SUT C99 Comprehensive Test", on_setup, on_draw, &app);
    return 0;
}
