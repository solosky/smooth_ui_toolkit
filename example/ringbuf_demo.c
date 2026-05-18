#include <raylib.h>
#include <stdlib.h>
#include "mc_ringbuf.h"
#include "raylib_helper.h"

#define WINDOW_SIZE 600
#define SAMPLE_RATE 60
#define TRAIL_SECONDS 3

typedef struct {
    mc_ringbuf_t buf;
    float phase;
    int display_mode;
} app_t;

static float wave_func(float phase, int mode) {
    switch (mode) {
        case 0: return sinf(phase);
        case 1: return sinf(phase * 3) * 0.5f + sinf(phase * 7) * 0.3f;
        case 2:
            phase = fmodf(phase, 2 * PI);
            return phase < PI ? 1.0f : -1.0f;
        case 3: return (float)rand() / RAND_MAX * 2 - 1;
        default: return 0;
    }
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_ringbuf_init(&app->buf, sizeof(float), SAMPLE_RATE * TRAIL_SECONDS);
    app->phase = 0;
    app->display_mode = 0;
    srand(42);
    float v = 0;
    for (int i = 0; i < SAMPLE_RATE * TRAIL_SECONDS; i++)
        mc_ringbuf_push(&app->buf, &v);
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    float dt = GetFrameTime();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    if (IsKeyPressed(KEY_SPACE))
        app->display_mode = (app->display_mode + 1) % 4;

    app->phase += dt * 2.0f;
    float sample = wave_func(app->phase, app->display_mode);
    mc_ringbuf_push(&app->buf, &sample);

    ClearBackground(BLACK);

    int plot_y = sh / 2;
    int plot_h = sh / 3;
    int n = mc_ringbuf_count(&app->buf);

    for (int i = 1; i < n; i++) {
        float v0, v1;
        mc_ringbuf_peek(&app->buf, i - 1, &v0);
        mc_ringbuf_peek(&app->buf, i, &v1);
        int x0 = (i - 1) * sw / n;
        int x1 = i * sw / n;
        int y0 = plot_y - (int)(v0 * plot_h / 2);
        int y1 = plot_y - (int)(v1 * plot_h / 2);
        Color c = (Color){ 0, 200, 255, (uint8_t)(200 - i * 150 / n) };
        DrawLine(x0, y0, x1, y1, c);
    }

    DrawLine(0, plot_y, sw, plot_y, (Color){40, 40, 50, 255});

    const char* modes[] = { "Sine Wave", "Harmonics", "Square Wave", "Noise" };
    DrawText("Ring Buffer Data Stream", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Mode: %s  (SPACE to change)", modes[app->display_mode]), 10, 40, 14, LIGHTGRAY);
    DrawText(TextFormat("Buffer: %d / %d samples", n, SAMPLE_RATE * TRAIL_SECONDS), 10, 60, 14, DARKGRAY);
    DrawText(TextFormat("Latest: %.3f", sample), 10, 80, 14, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 300, "Ring Buffer Demo - MotionC", on_setup, on_draw, &app);
    mc_ringbuf_deinit(&app.buf);
    return 0;
}
