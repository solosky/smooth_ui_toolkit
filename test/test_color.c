#include <stdio.h>
#include "mc_color.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_hex_conversion() {
    TEST("hex round-trip");
    mc_rgb_t c = mc_rgb_from_hex(0xFF8800);
    CHECK(c.r == 0xFF); CHECK(c.g == 0x88); CHECK(c.b == 0x00);
    CHECK(mc_rgb_to_hex(c) == 0xFF8800);
    PASS(); return 0;
}

static int test_hex_black_white() {
    TEST("hex black (0x000000) and white (0xFFFFFF)");
    mc_rgb_t black = mc_rgb_from_hex(0x000000);
    CHECK(black.r == 0 && black.g == 0 && black.b == 0);
    CHECK(mc_rgb_to_hex(black) == 0x000000);
    mc_rgb_t white = mc_rgb_from_hex(0xFFFFFF);
    CHECK(white.r == 0xFF && white.g == 0xFF && white.b == 0xFF);
    CHECK(mc_rgb_to_hex(white) == 0xFFFFFF);
    PASS(); return 0;
}

static int test_blend_opacity_bounds() {
    TEST("blend opacity 0%, 50%, 100%");
    mc_rgb_t bg = {100, 100, 100};
    mc_rgb_t fg = {200, 200, 200};
    mc_rgb_t r0 = mc_rgb_blend_opacity(bg, fg, MC_FP_C(0));
    CHECK(r0.r == 100 && r0.g == 100 && r0.b == 100);
    mc_rgb_t r50 = mc_rgb_blend_opacity(bg, fg, MC_FP_C(0.5f));
    CHECK(r50.r == 150 && r50.g == 150 && r50.b == 150);
    mc_rgb_t r1 = mc_rgb_blend_opacity(bg, fg, MC_FP_C(1));
    CHECK(r1.r == 200 && r1.g == 200 && r1.b == 200);
    PASS(); return 0;
}

static int test_blend_difference() {
    TEST("blend difference");
    mc_rgb_t a = {200, 50, 100};
    mc_rgb_t b = {100, 150, 200};
    mc_rgb_t d = mc_rgb_blend_difference(a, b);
    CHECK(d.r == 100 && d.g == 100 && d.b == 100);
    PASS(); return 0;
}

static int test_hsv_red() {
    TEST("hsv(0,100%,100%) = red");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 255); CHECK(c.g == 0); CHECK(c.b == 0);
    PASS(); return 0;
}

static int test_hsv_green() {
    TEST("hsv(120,100%,100%) = green");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(120), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 0); CHECK(c.g == 255); CHECK(c.b == 0);
    PASS(); return 0;
}

static int test_hsv_blue() {
    TEST("hsv(240,100%,100%) = blue");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(240), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 0); CHECK(c.g == 0); CHECK(c.b == 255);
    PASS(); return 0;
}

static int test_hsv_black() {
    TEST("hsv(0,0,0%) = black");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(0), MC_FP_C(0));
    CHECK(c.r == 0 && c.g == 0 && c.b == 0);
    PASS(); return 0;
}

static int test_hsv_zero_saturation() {
    TEST("hsv(any hue, 0% sat, 50% val) ≈ gray(127)");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(0), MC_FP_C(0.5f));
    CHECK(c.r >= 127 && c.r <= 128 && c.g >= 127 && c.g <= 128 && c.b >= 127 && c.b <= 128);
    c = mc_rgb_from_hsv(MC_FP_C(180), MC_FP_C(0), MC_FP_C(0.5f));
    CHECK(c.r >= 127 && c.r <= 128 && c.g >= 127 && c.g <= 128 && c.b >= 127 && c.b <= 128);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_hex_conversion();
    failed += test_hex_black_white();
    failed += test_blend_opacity_bounds();
    failed += test_blend_difference();
    failed += test_hsv_red();
    failed += test_hsv_green();
    failed += test_hsv_blue();
    failed += test_hsv_black();
    failed += test_hsv_zero_saturation();
    printf("Color: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
