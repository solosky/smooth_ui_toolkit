#include <stdio.h>
#include "mc_easing.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  TEST: %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(cond) do { if (!(cond)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_linear_bounds() {
    TEST("linear bounds");
    CHECK(mc_ease_linear(MC_FP_C(0)) == MC_FP_C(0));
    CHECK(mc_ease_linear(MC_FP_C(1)) == MC_FP_C(1));
    CHECK(mc_ease_linear(MC_FP_C(0.5f)) == MC_FP_C(0.5f));
    PASS(); return 0;
}

static int test_quad_out_bounds() {
    TEST("quad_out bounds");
    CHECK(mc_ease_quad_out(MC_FP_C(0)) == MC_FP_C(0));
    CHECK(mc_ease_quad_out(MC_FP_C(1)) == MC_FP_C(1));
    PASS(); return 0;
}

static int test_lut_bounds() {
    TEST("lut bounds");
    CHECK(mc_ease_sine_in(MC_FP_C(0)) == MC_FP_C(0));
    CHECK(mc_ease_sine_out(MC_FP_C(1)) == MC_FP_C(1));
    CHECK(mc_ease_bounce_out(MC_FP_C(0)) == MC_FP_C(0));
    CHECK(mc_ease_bounce_out(MC_FP_C(1)) == MC_FP_C(1));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_linear_bounds();
    failed += test_quad_out_bounds();
    failed += test_lut_bounds();
    printf("Easing: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
