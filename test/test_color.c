#include <stdio.h>
#include "mc_color.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_hex_conversion() {
    TEST("hex round-trip 0xFF8800");
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

static int test_hex_magenta_cyan() {
    TEST("hex magenta 0xFF00FF and cyan 0x00FFFF");
    mc_rgb_t m = mc_rgb_from_hex(0xFF00FF);
    CHECK(m.r == 0xFF && m.g == 0x00 && m.b == 0xFF);
    CHECK(mc_rgb_to_hex(m) == 0xFF00FF);
    mc_rgb_t c = mc_rgb_from_hex(0x00FFFF);
    CHECK(c.r == 0x00 && c.g == 0xFF && c.b == 0xFF);
    CHECK(mc_rgb_to_hex(c) == 0x00FFFF);
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

static int test_blend_opacity_clamped() {
    TEST("blend opacity clamped at extremes (<0 treated as 0, >1 treated as 1)");
    mc_rgb_t bg = {100, 100, 100};
    mc_rgb_t fg = {200, 200, 200};
    mc_rgb_t r = mc_rgb_blend_opacity(bg, fg, MC_FP_C(-1));
    CHECK(r.r == 100);
    r = mc_rgb_blend_opacity(bg, fg, MC_FP_C(2));
    CHECK(r.r == 200);
    PASS(); return 0;
}

static int test_blend_difference() {
    TEST("blend difference (200,50,100) x (100,150,200) = (100,100,100)");
    mc_rgb_t a = {200, 50, 100};
    mc_rgb_t b = {100, 150, 200};
    mc_rgb_t d = mc_rgb_blend_difference(a, b);
    CHECK(d.r == 100 && d.g == 100 && d.b == 100);
    PASS(); return 0;
}

static int test_blend_difference_identical() {
    TEST("blend difference of identical colors = (0,0,0)");
    mc_rgb_t a = {120, 80, 200};
    mc_rgb_t d = mc_rgb_blend_difference(a, a);
    CHECK(d.r == 0 && d.g == 0 && d.b == 0);
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

static int test_hsv_yellow() {
    TEST("hsv(60,100%,100%) = yellow");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(60), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 255); CHECK(c.g == 255); CHECK(c.b == 0);
    PASS(); return 0;
}

static int test_hsv_cyan() {
    TEST("hsv(180,100%,100%) = cyan");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(180), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 0); CHECK(c.g == 255); CHECK(c.b == 255);
    PASS(); return 0;
}

static int test_hsv_black() {
    TEST("hsv(any,any,0%) = black");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(1), MC_FP_C(0));
    CHECK(c.r == 0 && c.g == 0 && c.b == 0);
    PASS(); return 0;
}

static int test_hsv_zero_saturation() {
    TEST("hsv(any hue, 0% sat, V%) ≈ gray(V*255)");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(0), MC_FP_C(0.5f));
    CHECK(c.r >= 127 && c.r <= 128 && c.g >= 127 && c.g <= 128 && c.b >= 127 && c.b <= 128);
    c = mc_rgb_from_hsv(MC_FP_C(180), MC_FP_C(0), MC_FP_C(0.5f));
    CHECK(c.r >= 127 && c.r <= 128);
    PASS(); return 0;
}

static int test_hsv_partial_saturation() {
    TEST("hsv(0, 50%, 100%) = pinkish red");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(0), MC_FP_C(0.5f), MC_FP_C(1));
    CHECK(c.r == 255);
    CHECK(c.g >= 125 && c.g <= 130);
    CHECK(c.b >= 125 && c.b <= 130);
    PASS(); return 0;
}

static int test_hsv_magenta() {
    TEST("hsv(300, 100%, 100%) = magenta (case 5)");
    mc_rgb_t c = mc_rgb_from_hsv(MC_FP_C(300), MC_FP_C(1), MC_FP_C(1));
    CHECK(c.r == 255); CHECK(c.g == 0); CHECK(c.b == 255);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_hex_conversion();
    failed += test_hex_black_white();
    failed += test_hex_magenta_cyan();
    failed += test_blend_opacity_bounds();
    failed += test_blend_opacity_clamped();
    failed += test_blend_difference();
    failed += test_blend_difference_identical();
    failed += test_hsv_red();
    failed += test_hsv_green();
    failed += test_hsv_blue();
    failed += test_hsv_yellow();
    failed += test_hsv_cyan();
    failed += test_hsv_black();
    failed += test_hsv_zero_saturation();
    failed += test_hsv_partial_saturation();
    failed += test_hsv_magenta();
    printf("Color: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
