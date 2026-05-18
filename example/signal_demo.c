#include <raylib.h>
#include <string.h>
#include <stdlib.h>
#include "mc_signal.h"
#include "raylib_helper.h"

typedef struct {
    mc_signal_t click_sig;
    mc_signal_t hover_sig;
    int click_count;
    int hover_count;
    int listeners;
} app_t;

typedef struct { int x, y; } mouse_event_t;

static void on_click(void* ctx, void* args) {
    app_t* app = (app_t*)ctx;
    app->click_count++;
}

static void on_hover(void* ctx, void* args) {
    app_t* app = (app_t*)ctx;
    app->hover_count++;
}

static void add_listener(void* ctx, void* args) {
    app_t* app = (app_t*)ctx;
    if (mc_signal_connect(&app->click_sig, on_click, app) == MC_OK) {
        app->listeners++;
    }
}

static void remove_listener(void* ctx, void* args) {
    app_t* app = (app_t*)ctx;
    if (app->listeners > 0) {
        mc_signal_disconnect(&app->click_sig, on_click, app);
        app->listeners--;
    }
}

static void on_setup(void* ctx) {
    app_t* app = (app_t*)ctx;
    mc_signal_init(&app->click_sig, 20);
    mc_signal_init(&app->hover_sig, 20);
    app->click_count = 0;
    app->hover_count = 0;
    app->listeners = 0;
}

static void on_draw(void* ctx) {
    app_t* app = (app_t*)ctx;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int mx = GetMouseX();
    int my = GetMouseY();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        mc_signal_emit(&app->click_sig, NULL);

    Rectangle btn_rect = { 50, 100, 200, 50 };
    Rectangle btn_rect2 = { 50, 170, 200, 50 };

    bool hover_add = CheckCollisionPointRec((Vector2){ (float)mx, (float)my }, btn_rect);
    bool hover_rm = CheckCollisionPointRec((Vector2){ (float)mx, (float)my }, btn_rect2);

    if (hover_add) mc_signal_emit(&app->hover_sig, NULL);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover_add) add_listener(app, NULL);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover_rm) remove_listener(app, NULL);

    ClearBackground(BLACK);

    DrawRectangleRec(btn_rect, hover_add ? DARKGREEN : (Color){30, 30, 40, 255});
    DrawRectangleLinesEx(btn_rect, 2, LIGHTGRAY);
    DrawText("Add Listener +", 90, 115, 20, RAYWHITE);

    DrawRectangleRec(btn_rect2, hover_rm ? DARKBROWN : (Color){30, 30, 40, 255});
    DrawRectangleLinesEx(btn_rect2, 2, LIGHTGRAY);
    DrawText("Remove Listener -", 75, 185, 20, RAYWHITE);

    DrawText("Signal/Slot Demo", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Active listeners: %d", app->listeners), 10, 40, 16, LIGHTGRAY);
    DrawText(TextFormat("Click events fired: %d", app->click_count), 10, 60, 16, LIGHTGRAY);
    DrawText("Click anywhere to fire signal", 10, 260, 14, DARKGRAY);

    int bx = 400;
    int by = 100;
    int spacing = 30;
    for (int i = 0; i < app->listeners && i < 15; i++) {
        DrawCircle(bx + i * 18, by, 6, (Color){ 0, 200, 80, 200 });
    }
    if (app->listeners > 0)
        DrawText("Listener indicator (each green dot = 1 listener)", bx, by + 15, 12, DARKGRAY);
}

int main(void) {
    app_t app;
    sut_example_run(800, 330, "Signal Demo - MotionC", on_setup, on_draw, &app);
    mc_signal_deinit(&app.click_sig);
    mc_signal_deinit(&app.hover_sig);
    return 0;
}
