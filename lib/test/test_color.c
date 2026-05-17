#include <stdio.h>
#include "sut_color.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_hex_conversion() {
    TEST("hex round-trip");
    sut_rgb_t c = sut_rgb_from_hex(0xFF8800);
    CHECK(c.r == 0xFF); CHECK(c.g == 0x88); CHECK(c.b == 0x00);
    CHECK(sut_rgb_to_hex(c) == 0xFF8800);
    PASS(); return 0;
}

static int test_blend_opacity_bounds() {
    TEST("blend opacity 0% and 100%");
    sut_rgb_t bg = {100, 100, 100};
    sut_rgb_t fg = {200, 200, 200};
    sut_rgb_t r0 = sut_rgb_blend_opacity(bg, fg, SUT_FP_C(0));
    CHECK(r0.r == 100 && r0.g == 100 && r0.b == 100);
    sut_rgb_t r1 = sut_rgb_blend_opacity(bg, fg, SUT_FP_C(1));
    CHECK(r1.r == 200 && r1.g == 200 && r1.b == 200);
    PASS(); return 0;
}

static int test_hsv_red() {
    TEST("hsv(0,100%,100%) = red");
    sut_rgb_t c = sut_rgb_from_hsv(SUT_FP_C(0), SUT_FP_C(1), SUT_FP_C(1));
    CHECK(c.r == 255); CHECK(c.g == 0); CHECK(c.b == 0);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_hex_conversion();
    failed += test_blend_opacity_bounds();
    failed += test_hsv_red();
    printf("Color: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
