#include <stdio.h>
#include "mc_easing.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(name) do { tests_run++; printf("  TEST: %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c)   do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_linear_bounds() {
    TEST("linear t=0, 0.5, 1");
    CHECK(MC_REAL_TO_INT(mc_ease_linear(MC_FP_C(0))) == 0);
    CHECK(MC_REAL_TO_INT(mc_ease_linear(MC_FP_C(0.5f))) == 0);
    CHECK(MC_REAL_TO_INT(mc_ease_linear(MC_FP_C(1))) == 1);
    PASS(); return 0;
}

static int test_all_polynomial_bounds() {
    TEST("all 12 polynomial eases at t=0 and t=1");
    mc_easing_fn_t fns[] = {
        mc_ease_quad_in, mc_ease_quad_out, mc_ease_quad_in_out,
        mc_ease_cubic_in, mc_ease_cubic_out, mc_ease_cubic_in_out,
        mc_ease_quart_in, mc_ease_quart_out, mc_ease_quart_in_out,
        mc_ease_quint_in, mc_ease_quint_out, mc_ease_quint_in_out,
    };
    for (int i = 0; i < 12; i++) {
        CHECK(MC_REAL_TO_INT(fns[i](MC_FP_C(0))) == 0);
        CHECK(MC_REAL_TO_INT(fns[i](MC_FP_C(1))) == 1);
    }
    PASS(); return 0;
}

static int test_polynomial_monotonic() {
    TEST("polynomial eases are monotonic");
    mc_easing_fn_t fns[] = {
        mc_ease_quad_in, mc_ease_quad_out, mc_ease_cubic_in, mc_ease_cubic_out,
        mc_ease_quart_in, mc_ease_quart_out, mc_ease_quint_in, mc_ease_quint_out,
    };
    for (int i = 0; i < 8; i++) {
        mc_real_t prev = fns[i](MC_FP_C(0));
        for (int j = 1; j <= 100; j++) {
            mc_real_t cur = fns[i](MC_FP_C(j / 100.0f));
            CHECK(cur >= prev);
            prev = cur;
        }
    }
    PASS(); return 0;
}

static int test_in_out_at_half() {
    TEST("in_out eases at t=0.5 equal 0.5 (symmetric)");
    mc_easing_fn_t fns[] = {
        mc_ease_quad_in_out, mc_ease_cubic_in_out,
        mc_ease_quart_in_out, mc_ease_quint_in_out,
    };
    for (int i = 0; i < 4; i++) {
        mc_real_t v = fns[i](MC_FP_C(0.5f));
        CHECK(v >= MC_FP_C(0.48f) && v <= MC_FP_C(0.52f));
    }
    PASS(); return 0;
}

static int test_all_lut_bounds() {
    TEST("all LUT eases at t=0 and t=1");
    mc_easing_fn_t fns[] = {
        mc_ease_sine_in, mc_ease_sine_out, mc_ease_sine_in_out,
        mc_ease_expo_in, mc_ease_expo_out, mc_ease_expo_in_out,
        mc_ease_circ_in, mc_ease_circ_out, mc_ease_circ_in_out,
        mc_ease_back_in, mc_ease_back_out, mc_ease_back_in_out,
        mc_ease_elastic_in, mc_ease_elastic_out, mc_ease_elastic_in_out,
        mc_ease_bounce_in, mc_ease_bounce_out, mc_ease_bounce_in_out,
    };
    for (int i = 0; i < 18; i++) {
        CHECK(MC_REAL_TO_INT(fns[i](MC_FP_C(0))) == 0);
        CHECK(MC_REAL_TO_INT(fns[i](MC_FP_C(1))) == 1);
    }
    PASS(); return 0;
}

static int test_overbounds_behaviour() {
    TEST("elastic and back eases go beyond [0,1] range (overshoot)");
    mc_real_t e = mc_ease_back_in(MC_FP_C(0.5f));
    CHECK(e < MC_FP_C(0));
    e = mc_ease_back_out(MC_FP_C(0.5f));
    CHECK(e > MC_FP_C(1));
    e = mc_ease_elastic_out(MC_FP_C(0.5f));
    CHECK(e > MC_FP_C(1));
    PASS(); return 0;
}

static int test_cubic_bezier() {
    TEST("cubic_bezier t=0→0, t=1→1, mid in (0,1)");
    mc_real_t bz0 = mc_ease_cubic_bezier(MC_FP_C(0), MC_FP_C(0.25f), MC_FP_C(0.1f), MC_FP_C(0.75f), MC_FP_C(0.9f));
    CHECK(bz0 < MC_FP_C(0.01f));
    mc_real_t bz1 = mc_ease_cubic_bezier(MC_FP_C(1), MC_FP_C(0.25f), MC_FP_C(0.1f), MC_FP_C(0.75f), MC_FP_C(0.9f));
    CHECK(bz1 >= MC_FP_C(0.99f));
    mc_real_t mid = mc_ease_cubic_bezier(MC_FP_C(0.5f), MC_FP_C(0.42f), MC_FP_C(0), MC_FP_C(0.58f), MC_FP_C(1));
    CHECK(mid > MC_FP_C(0) && mid < MC_FP_C(1));
    PASS(); return 0;
}

static int test_cubic_bezier_ease() {
    TEST("cubic_bezier with ease-in-out params is symmetric");
    mc_real_t mid = mc_ease_cubic_bezier(MC_FP_C(0.5f), MC_FP_C(0.42f), MC_FP_C(0), MC_FP_C(0.58f), MC_FP_C(1));
    CHECK(mid >= MC_FP_C(0.48f) && mid <= MC_FP_C(0.52f));
    PASS(); return 0;
}

static int test_cubic_bezier_linear_equivalent() {
    TEST("cubic_bezier(0,0,1,1) is linear");
    mc_real_t v = mc_ease_cubic_bezier(MC_FP_C(0.5f), MC_FP_C(0), MC_FP_C(0), MC_FP_C(1), MC_FP_C(1));
    CHECK(v >= MC_FP_C(0.48f) && v <= MC_FP_C(0.52f));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_linear_bounds();
    failed += test_all_polynomial_bounds();
    failed += test_polynomial_monotonic();
    failed += test_in_out_at_half();
    failed += test_all_lut_bounds();
    failed += test_overbounds_behaviour();
    failed += test_cubic_bezier();
    failed += test_cubic_bezier_ease();
    failed += test_cubic_bezier_linear_equivalent();
    printf("Easing: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
