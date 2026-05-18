#include <raylib.h>
#include "mc_animate.h"
#include "mc_sequence.h"
#include "raylib_helper.h"

typedef struct {
    mc_sequence_t seq;
    mc_real_t current_val;
    int step_index;
    int complete;
} app_t;

static void on_step(void* ctx, int index) {
    app_t* app = (app_t*)ctx;
    app->step_index = index;
}

static void on_complete(void* ctx) {
    app_t* app = (app_t*)ctx;
    app->complete = 1;
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_sequence_init(&app->seq, 8);
    app->seq.on_step = on_step;
    app->seq.on_complete = on_complete;
    app->seq.ctx = app;

    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(100), MC_FP_C(0.5f), mc_ease_quad_out });
    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(300), MC_FP_C(0.8f), mc_ease_bounce_out });
    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(500), MC_FP_C(0.6f), mc_ease_elastic_out });
    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(200), MC_FP_C(0.4f), mc_ease_cubic_in_out });
    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(600), MC_FP_C(0.7f), mc_ease_back_out });
    mc_sequence_push(&app->seq, (mc_animate_step_t){ MC_FP_C(50),  MC_FP_C(0.5f), mc_ease_quad_in });

    mc_sequence_play(&app->seq);
    app->step_index = 0;
    app->complete = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    if (IsKeyPressed(KEY_SPACE)) {
        mc_sequence_play(&app->seq);
        app->step_index = 0;
        app->complete = 0;
    }

    bool done = mc_sequence_update(&app->seq, MC_REAL_FROM_FLOAT(dt));

    if (app->seq.count > 0 && !done) {
        mc_animate_step_t* cur = &app->seq.steps[app->seq.step_index];
        float progress = cur->duration > 0
            ? (float)(MC_REAL_TO_FLOAT(app->seq.elapsed) / MC_REAL_TO_FLOAT(cur->duration))
            : 0;
        if (progress > 1) progress = 1;

        mc_real_t t = mc_lerp(MC_FP_C(0), MC_FP_C(1), MC_REAL_FROM_FLOAT(progress));
        mc_real_t eased = cur->easing(t);
        mc_real_t from = app->seq.step_index > 0
            ? app->seq.steps[app->seq.step_index - 1].target
            : 0;
        app->current_val = mc_lerp(from, cur->target, eased);
    }

    int bar_x = 100;
    int bar_y = sh / 2;
    int bar_w = sw - 200;
    int bar_h = 40;

    DrawRectangle(bar_x, bar_y, bar_w, bar_h, (Color){30, 30, 40, 255});
    DrawRectangleLines(bar_x, bar_y, bar_w, bar_h, (Color){60, 60, 80, 255});

    int fill_w = app->seq.count > 0
        ? (int)((float)app->current_val / 700.0f * bar_w) : 0;
    DrawRectangle(bar_x + 1, bar_y + 1, fill_w, bar_h - 2, (Color){0, 180, 255, 200});

    int dot_y = sh / 2 - 60;
    for (unsigned int i = 0; i < app->seq.count; i++) {
        int tx = bar_x + (int)(MC_REAL_TO_FLOAT(app->seq.steps[i].target) / 700.0f * bar_w);
        Color c = DARKGRAY;
        if (i == app->step_index && !done) c = GREEN;
        else if (i < app->step_index) c = (Color){0, 180, 80, 255};
        DrawCircle(tx, dot_y, 8, c);
        DrawText(TextFormat("S%d", i + 1), tx - 8, dot_y + 14, 10, LIGHTGRAY);
    }

    DrawText("Animation Sequence Demo", 10, 10, 20, RAYWHITE);
    DrawText("Press SPACE to replay", 10, 35, 14, DARKGRAY);
    DrawText(TextFormat("Step %d / %d  %s",
        app->step_index + 1, app->seq.count,
        done ? "COMPLETE" : "running"),
        10, 60, 14, LIGHTGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 450, "Animation Sequence - MotionC", on_setup, on_draw, &app);
    return 0;
}
