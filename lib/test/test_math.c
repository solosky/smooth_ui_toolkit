#include <stdio.h>
#include "sut_math.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  TEST: %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(cond) do { if (!(cond)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_clamp() {
    TEST("clamp");
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(5), SUT_FP_C(0), SUT_FP_C(10))) == 5);
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(-1), SUT_FP_C(0), SUT_FP_C(10))) == 0);
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(15), SUT_FP_C(0), SUT_FP_C(10))) == 10);
    PASS(); return 0;
}

static int test_lerp() {
    TEST("lerp 0->10 at t=0.5");
    sut_real_t r = sut_lerp(SUT_FP_C(0), SUT_FP_C(10), SUT_FP_C(0.5f));
    CHECK(SUT_REAL_TO_INT(r) == 5);
    PASS(); return 0;
}

static int test_fp_mul() {
    TEST("fp_mul 3 * 4");
    sut_real_t r = sut_fp_mul(SUT_FP_C(3), SUT_FP_C(4));
    CHECK(SUT_REAL_TO_INT(r) == 12);
    PASS(); return 0;
}

static int test_fp_div() {
    TEST("fp_div 10 / 2");
    sut_real_t r = sut_fp_div(SUT_FP_C(10), SUT_FP_C(2));
    CHECK(SUT_REAL_TO_INT(r) == 5);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_clamp();
    failed += test_lerp();
    failed += test_fp_mul();
    failed += test_fp_div();
    printf("Math: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
