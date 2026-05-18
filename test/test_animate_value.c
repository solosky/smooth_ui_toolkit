#include <stdio.h>
#include "sut_animate.h"
#include "sut_math.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(name) do { tests_run++; printf("  %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c)   do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

/*
 * Test: fp_mul overflow detection
 *
 * In Q16.16 fixed-point mode:
 *   SUT_FP_C(120) = 120 * 65536 = 7864320
 *   SUT_FP_C(300) = 300 * 65536 = 19660800
 *   sut_fp_mul(7864320, 19660800) produces 300*120=36000 * 65536 = 2359296000
 *   But 2359296000 > INT32_MAX (2147483647) !! OVERFLOW !!
 *
 * In the animate_value.c example:
 *   stiffness = 120, displacement can be up to ~400 (window width)
 *   120 * 400 = 48000. 48000 * 65536 = 3145728000 >> INT32_MAX
 *   Result: int32_t overflow, producing garbage (negative) values
 */
static int test_fp_mul_no_overflow_for_small() {
    TEST("fp_mul small values (10 * 20) - no overflow expected");
    sut_real_t r = sut_fp_mul(SUT_FP_C(10), SUT_FP_C(20));
    CHECK(SUT_REAL_TO_INT(r) == 200);
    PASS(); return 0;
}

static int test_fp_mul_large_stiffness_displacement() {
    TEST("fp_mul: stiffness=120 * displacement=300 (exceeds Q16.16 range)");
    sut_real_t force = sut_fp_mul(SUT_FP_C(120), SUT_FP_C(300));
    int actual = SUT_REAL_TO_INT(force);
    /* 120*300=36000 > 32767 (Q16.16 max). Must be saturated, not negative. */
    CHECK(actual > 0);
    printf("actual=%d (saturated) ", actual);
    PASS(); return 0;
}

static int test_fp_mul_400_displacement() {
    TEST("fp_mul: stiffness=120 * displacement=400 (must not overflow)");
    sut_real_t force = sut_fp_mul(SUT_FP_C(120), SUT_FP_C(400));
    int actual = SUT_REAL_TO_INT(force);
    /* Must be positive (saturated), not negative garbage */
    CHECK(actual > 0);
    printf("actual=%d (saturated) ", actual);
    PASS(); return 0;
}

static int test_fp_mul_increasing_displacement() {
    TEST("fp_mul: stiffness=120 with displacement 0..400 (all positive)");
    for (int d = 0; d <= 400; d += 50) {
        sut_real_t f = sut_fp_mul(SUT_FP_C(120), SUT_FP_C(d));
        int actual = SUT_REAL_TO_INT(f);
        if (actual < 0) {
            printf("\n  FAIL: d=%d actual=%d (negative!)\n", d, actual);
            return 1;
        }
    }
    PASS(); return 0;
}

static int test_fp_div_large_safe() {
    TEST("fp_div: large value / 1.0 (test unsigned math path)");
    sut_real_t v = SUT_FP_C(500);
    sut_real_t r = sut_fp_div(v, SUT_FP_C(1));
    CHECK(SUT_REAL_TO_INT(r) == 500);
    PASS(); return 0;
}

static int test_fp_div_negative() {
    TEST("fp_div: -100 / 2");
    sut_real_t r = sut_fp_div(SUT_FP_C(-100), SUT_FP_C(2));
    CHECK(SUT_REAL_TO_INT(r) == -50);
    PASS(); return 0;
}

static int test_fp_div_by_large() {
    TEST("fp_div: 1 / 100");
    sut_real_t r = sut_fp_div(SUT_FP_C(1), SUT_FP_C(100));
    CHECK(r >= 0);
    PASS(); return 0;
}

static int test_fp_mul_negative() {
    TEST("fp_mul: -30 * 4");
    sut_real_t r = sut_fp_mul(SUT_FP_C(-30), SUT_FP_C(4));
    CHECK(SUT_REAL_TO_INT(r) == -120);
    PASS(); return 0;
}

static int test_fp_mul_negative_both() {
    TEST("fp_mul: -30 * -4");
    sut_real_t r = sut_fp_mul(SUT_FP_C(-30), SUT_FP_C(-4));
    CHECK(SUT_REAL_TO_INT(r) == 120);
    PASS(); return 0;
}

/*
 * Spring step test: simulate animate_value.c scenario
 * Starting from x=400 (center), target=0 (left edge click).
 * With stiffness=120, damping=12, mass=1.
 */
static int test_spring_step_from_center_to_zero() {
    TEST("spring_step: 400->0 with stiffness=120 damping=12 dt=1/60");

    /* Verify force multiplication is safe (saturated, not negative) */
    sut_real_t displacement = SUT_FP_C(400) - SUT_FP_C(0);
    sut_real_t force_mag = sut_fp_mul(SUT_FP_C(120), displacement);
    int force_int = SUT_REAL_TO_INT(force_mag);
    if (force_int < 0) {
        printf("FORCE NEGATIVE: 120*400 should be >=0 but got %d ", force_int);
        CHECK(0 && "fp_mul returned negative for positive operands!");
    }

    sut_real_t x = SUT_FP_C(400);
    sut_spring_state_t s = { 0 };
    sut_spring_params_t p = {
        SUT_FP_C(120),
        SUT_FP_C(12),
        SUT_FP_C(1)
    };
    sut_real_t target = SUT_FP_C(0);
    sut_real_t dt = SUT_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Run many frames, check for convergence or near zero */
    for (int i = 0; i < 300; i++) {
        sut_spring_step(&x, &s, &p, target, dt);
        /* After a few frames, x should still be reasonable */
        if (i > 10) {
            int xi = SUT_REAL_TO_INT(x);
            /* x should not exceed sane bounds */
            if (xi < -200 || xi > 500) {
                printf("\n  FAIL: frame=%d x=%d (out of range)\n", i, xi);
                return 1;
            }
        }
    }

    /* After 300 frames (5s), should be near 0 */
    sut_real_t error = x - target;
    if (error < 0) error = -error;
    int ei = SUT_REAL_TO_INT(error);
    printf("final error=%d ", ei);
    CHECK(ei < 10);
    PASS(); return 0;
}

/*
 * Full simulation: sut_animate_t spring mode, click to retarget
 * mimicking the exact animate_value.c pattern.
 */
static int test_spring_anim_retarget() {
    TEST("sut_animate spring: init at 400, retarget to click positions");

    sut_animate_t anim_x;
    sut_animate_init_spring(&anim_x, SUT_FP_C(400), SUT_FP_C(400));
    anim_x.config.spring.stiffness = SUT_FP_C(120);
    anim_x.config.spring.damping = SUT_FP_C(12);

    sut_real_t dt = SUT_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Let it settle for 120 frames */
    for (int i = 0; i < 120; i++) {
        sut_animate_update(&anim_x, dt);
    }

    /* Click 1: target = 100 (left side) */
    sut_real_t targets[] = {
        SUT_FP_C(100),
        SUT_FP_C(600),
        SUT_FP_C(50),
        SUT_FP_C(700),
        SUT_FP_C(10),
    };

    for (int click = 0; click < 5; click++) {
        /* Retarget: from=current, to=mouse, velocity=0, elapsed=0 */
        anim_x.to = targets[click];
        anim_x.from = anim_x.current;
        anim_x.velocity = 0;
        anim_x.elapsed = 0;

        /* Run for 120 frames, checking x never goes wildly negative */
        for (int i = 0; i < 120; i++) {
            sut_animate_update(&anim_x, dt);
            int xi = SUT_REAL_TO_INT(anim_x.current);
            if (xi < -500 || xi > 1500) {
                printf("\n  FAIL: click=%d frame=%d x=%d (wild value)\n",
                       click, i, xi);
                return 1;
            }
        }
    }

    PASS(); return 0;
}

/*
 * Check that the raw current value never goes below
 * a reasonable bound during spring animation with retargeting.
 */
static int test_spring_anim_edge_target() {
    TEST("spring anim retarget to x=5 (very close to left edge)");

    sut_animate_t anim_x;
    sut_animate_init_spring(&anim_x, SUT_FP_C(400), SUT_FP_C(400));
    anim_x.config.spring.stiffness = SUT_FP_C(120);
    anim_x.config.spring.damping = SUT_FP_C(12);

    sut_real_t dt = SUT_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Settle */
    for (int i = 0; i < 60; i++) sut_animate_update(&anim_x, dt);

    /* Click at x=5 (near edge) */
    anim_x.to = SUT_REAL_FROM_INT(5);
    anim_x.from = anim_x.current;
    anim_x.velocity = 0;
    anim_x.elapsed = 0;

    for (int i = 0; i < 300; i++) {
        sut_animate_update(&anim_x, dt);
        int xi = SUT_REAL_TO_INT(anim_x.current);
        /* Overshoot might give small negative, but shouldn't be wild */
        if (xi < -100 || xi > 500) {
            printf("\n  FAIL: frame=%d x=%d (excessive overshoot)\n", i, xi);
            return 1;
        }
    }

    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_fp_mul_no_overflow_for_small();
    failed += test_fp_mul_large_stiffness_displacement();
    failed += test_fp_mul_400_displacement();
    failed += test_fp_mul_increasing_displacement();
    failed += test_fp_div_large_safe();
    failed += test_fp_div_negative();
    failed += test_fp_div_by_large();
    failed += test_fp_mul_negative();
    failed += test_fp_mul_negative_both();
    failed += test_spring_step_from_center_to_zero();
    failed += test_spring_anim_retarget();
    failed += test_spring_anim_edge_target();

    printf("AnimateValue: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
