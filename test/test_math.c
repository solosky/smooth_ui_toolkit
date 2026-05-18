#include <stdio.h>
#include "mc_math.h"

static int tests_run = 0, tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  TEST: %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c)   do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_clamp() {
    TEST("clamp within range, below min, above max");
    CHECK(MC_REAL_TO_INT(mc_clamp(MC_FP_C(5), MC_FP_C(0), MC_FP_C(10))) == 5);
    CHECK(MC_REAL_TO_INT(mc_clamp(MC_FP_C(-1), MC_FP_C(0), MC_FP_C(10))) == 0);
    CHECK(MC_REAL_TO_INT(mc_clamp(MC_FP_C(15), MC_FP_C(0), MC_FP_C(10))) == 10);
    PASS(); return 0;
}

static int test_lerp() {
    TEST("lerp 0->10 at t=0, 0.5, 1");
    CHECK(MC_REAL_TO_INT(mc_lerp(MC_FP_C(0), MC_FP_C(10), MC_FP_C(0))) == 0);
    CHECK(MC_REAL_TO_INT(mc_lerp(MC_FP_C(0), MC_FP_C(10), MC_FP_C(0.5f))) == 5);
    CHECK(MC_REAL_TO_INT(mc_lerp(MC_FP_C(0), MC_FP_C(10), MC_FP_C(1))) == 10);
    PASS(); return 0;
}

static int test_map() {
    TEST("map 0-10 -> 100-200 at 5");
    mc_real_t r = mc_map(MC_FP_C(5), MC_FP_C(0), MC_FP_C(10), MC_FP_C(100), MC_FP_C(200));
    CHECK(MC_REAL_TO_INT(r) == 150);
    PASS(); return 0;
}

static int test_fp_mul() {
    TEST("fp_mul 3 * 4 = 12");
    CHECK(MC_REAL_TO_INT(mc_fp_mul(MC_FP_C(3), MC_FP_C(4))) == 12);
    PASS(); return 0;
}

static int test_fp_mul_negative() {
    TEST("fp_mul -3 * 4 = -12, -3 * -4 = 12");
    CHECK(MC_REAL_TO_INT(mc_fp_mul(MC_FP_C(-3), MC_FP_C(4))) == -12);
    CHECK(MC_REAL_TO_INT(mc_fp_mul(MC_FP_C(-3), MC_FP_C(-4))) == 12);
    PASS(); return 0;
}

static int test_fp_mul_zero() {
    TEST("fp_mul with zero");
    CHECK(MC_REAL_TO_INT(mc_fp_mul(MC_FP_C(0), MC_FP_C(100))) == 0);
    CHECK(MC_REAL_TO_INT(mc_fp_mul(MC_FP_C(100), MC_FP_C(0))) == 0);
    PASS(); return 0;
}

static int test_fp_div() {
    TEST("fp_div 10 / 2 = 5");
    CHECK(MC_REAL_TO_INT(mc_fp_div(MC_FP_C(10), MC_FP_C(2))) == 5);
    PASS(); return 0;
}

static int test_fp_div_negative() {
    TEST("fp_div -10 / 2 = -5, 10 / -2 = -5, -10 / -2 = 5");
    CHECK(MC_REAL_TO_INT(mc_fp_div(MC_FP_C(-10), MC_FP_C(2))) == -5);
    CHECK(MC_REAL_TO_INT(mc_fp_div(MC_FP_C(10), MC_FP_C(-2))) == -5);
    CHECK(MC_REAL_TO_INT(mc_fp_div(MC_FP_C(-10), MC_FP_C(-2))) == 5);
    PASS(); return 0;
}

static int test_fp_div_by_zero() {
    TEST("fp_div by zero returns 0");
    CHECK(mc_fp_div(MC_FP_C(10), MC_FP_C(0)) == MC_FP_C(0));
    PASS(); return 0;
}

static int test_fp_sqrt() {
    TEST("fp_sqrt perfect squares 0, 1, 4, 9, 16, 100");
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(0))) == 0);
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(1))) == 1);
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(4))) == 2);
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(9))) == 3);
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(16))) == 4);
    CHECK(MC_REAL_TO_INT(mc_fp_sqrt(MC_FP_C(100))) == 10);
    PASS(); return 0;
}

static int test_fp_sqrt_negative() {
    TEST("fp_sqrt of negative returns 0");
    CHECK(mc_fp_sqrt(MC_FP_C(-1)) == MC_FP_C(0));
    PASS(); return 0;
}

static int test_vec2_ops() {
    TEST("vec2 add, sub, mul, dot, length, normalize, lerp");
    mc_vec2_t a = mc_vec2(MC_FP_C(3), MC_FP_C(4));
    mc_vec2_t b = mc_vec2(MC_FP_C(1), MC_FP_C(2));

    mc_vec2_t sum = mc_vec2_add(a, b);
    CHECK(MC_REAL_TO_INT(sum.x) == 4 && MC_REAL_TO_INT(sum.y) == 6);

    mc_vec2_t diff = mc_vec2_sub(a, b);
    CHECK(MC_REAL_TO_INT(diff.x) == 2 && MC_REAL_TO_INT(diff.y) == 2);

    mc_vec2_t scaled = mc_vec2_mul(a, MC_FP_C(2));
    CHECK(MC_REAL_TO_INT(scaled.x) == 6 && MC_REAL_TO_INT(scaled.y) == 8);

    mc_real_t dot = mc_vec2_dot(a, b);
    CHECK(MC_REAL_TO_INT(dot) == 11);

    mc_real_t len = mc_vec2_length(a);
    CHECK(len >= MC_FP_C(4) && len <= MC_FP_C(6));

    mc_vec2_t n = mc_vec2_normalize(a);
    mc_real_t nlen = mc_vec2_length(n);
    CHECK(nlen >= MC_FP_C(0.99f) && nlen <= MC_FP_C(1.01f));

    mc_vec2_t lerped = mc_vec2_lerp(a, b, MC_FP_C(0.5f));
    CHECK(MC_REAL_TO_INT(lerped.x) == 2 && MC_REAL_TO_INT(lerped.y) == 3);

    PASS(); return 0;
}

static int test_vec4_ops() {
    TEST("vec4 add, sub, mul, dot, lerp");
    mc_vec4_t a = mc_vec4(MC_FP_C(1), MC_FP_C(2), MC_FP_C(3), MC_FP_C(4));
    mc_vec4_t b = mc_vec4(MC_FP_C(5), MC_FP_C(6), MC_FP_C(7), MC_FP_C(8));

    mc_vec4_t sum = mc_vec4_add(a, b);
    CHECK(MC_REAL_TO_INT(sum.x) == 6 && MC_REAL_TO_INT(sum.y) == 8 &&
           MC_REAL_TO_INT(sum.z) == 10 && MC_REAL_TO_INT(sum.w) == 12);

    mc_vec4_t diff = mc_vec4_sub(a, b);
    CHECK(MC_REAL_TO_INT(diff.x) == -4 && MC_REAL_TO_INT(diff.y) == -4 &&
           MC_REAL_TO_INT(diff.z) == -4 && MC_REAL_TO_INT(diff.w) == -4);

    mc_vec4_t mul = mc_vec4_mul(a, MC_FP_C(2));
    CHECK(MC_REAL_TO_INT(mul.x) == 2 && MC_REAL_TO_INT(mul.y) == 4 &&
           MC_REAL_TO_INT(mul.z) == 6 && MC_REAL_TO_INT(mul.w) == 8);

    mc_real_t dot = mc_vec4_dot(a, b);
    CHECK(MC_REAL_TO_INT(dot) == 70);

    mc_vec4_t lerped = mc_vec4_lerp(a, b, MC_FP_C(0.5f));
    CHECK(MC_REAL_TO_INT(lerped.x) == 3 && MC_REAL_TO_INT(lerped.y) == 4 &&
           MC_REAL_TO_INT(lerped.z) == 5 && MC_REAL_TO_INT(lerped.w) == 6);

    PASS(); return 0;
}

static int test_rect_contains() {
    TEST("rect contains point");
    mc_rect_t r = { 0, 0, MC_FP_C(100), MC_FP_C(50) };
    CHECK(mc_rect_contains(r, mc_vec2(MC_FP_C(50), MC_FP_C(25))) == true);
    CHECK(mc_rect_contains(r, mc_vec2(MC_FP_C(-1), MC_FP_C(25))) == false);
    CHECK(mc_rect_contains(r, mc_vec2(MC_FP_C(50), MC_FP_C(60))) == false);
    PASS(); return 0;
}

static int test_rect_overlaps() {
    TEST("rect overlaps another rect");
    mc_rect_t a = { 0, 0, MC_FP_C(100), MC_FP_C(100) };
    mc_rect_t b = { MC_FP_C(50), MC_FP_C(50), MC_FP_C(100), MC_FP_C(100) };
    mc_rect_t c = { MC_FP_C(200), MC_FP_C(200), MC_FP_C(50), MC_FP_C(50) };
    CHECK(mc_rect_overlaps(a, b) == true);
    CHECK(mc_rect_overlaps(a, c) == false);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_clamp();
    failed += test_lerp();
    failed += test_map();
    failed += test_fp_mul();
    failed += test_fp_mul_negative();
    failed += test_fp_mul_zero();
    failed += test_fp_div();
    failed += test_fp_div_negative();
    failed += test_fp_div_by_zero();
    failed += test_fp_sqrt();
    failed += test_fp_sqrt_negative();
    failed += test_vec2_ops();
    failed += test_vec4_ops();
    failed += test_rect_contains();
    failed += test_rect_overlaps();
    printf("Math: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
