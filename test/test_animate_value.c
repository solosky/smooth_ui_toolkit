#include <stdio.h>
#include "mc_animate.h"
#include "mc_math.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(name) do { tests_run++; printf("  %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c)   do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

/*
 * Test: fp_mul overflow detection
 *
 * In Q16.16 fixed-point mode:
 *   MC_FP_C(120) = 120 * 65536 = 7864320
 *   MC_FP_C(300) = 300 * 65536 = 19660800
 *   mc_fp_mul(7864320, 19660800) produces 300*120=36000 * 65536 = 2359296000
 *   But 2359296000 > INT32_MAX (2147483647) !! OVERFLOW !!
 *
 * In the animate_value.c example:
 *   stiffness = 120, displacement can be up to ~400 (window width)
 *   120 * 400 = 48000. 48000 * 65536 = 3145728000 >> INT32_MAX
 *   Result: int32_t overflow, producing garbage (negative) values
 */
static int test_fp_mul_no_overflow_for_small() {
    TEST("fp_mul small values (10 * 20) - no overflow expected");
    mc_real_t r = mc_fp_mul(MC_FP_C(10), MC_FP_C(20));
    CHECK(MC_REAL_TO_INT(r) == 200);
    PASS(); return 0;
}

static int test_fp_mul_large_stiffness_displacement() {
    TEST("fp_mul: stiffness=120 * displacement=300 (exceeds Q16.16 range)");
    mc_real_t force = mc_fp_mul(MC_FP_C(120), MC_FP_C(300));
    int actual = MC_REAL_TO_INT(force);
    /* 120*300=36000 > 32767 (Q16.16 max). Must be saturated, not negative. */
    CHECK(actual > 0);
    printf("actual=%d (saturated) ", actual);
    PASS(); return 0;
}

static int test_fp_mul_400_displacement() {
    TEST("fp_mul: stiffness=120 * displacement=400 (must not overflow)");
    mc_real_t force = mc_fp_mul(MC_FP_C(120), MC_FP_C(400));
    int actual = MC_REAL_TO_INT(force);
    /* Must be positive (saturated), not negative garbage */
    CHECK(actual > 0);
    printf("actual=%d (saturated) ", actual);
    PASS(); return 0;
}

static int test_fp_mul_increasing_displacement() {
    TEST("fp_mul: stiffness=120 with displacement 0..400 (all positive)");
    for (int d = 0; d <= 400; d += 50) {
        mc_real_t f = mc_fp_mul(MC_FP_C(120), MC_FP_C(d));
        int actual = MC_REAL_TO_INT(f);
        if (actual < 0) {
            printf("\n  FAIL: d=%d actual=%d (negative!)\n", d, actual);
            return 1;
        }
    }
    PASS(); return 0;
}

static int test_fp_div_large_safe() {
    TEST("fp_div: large value / 1.0 (test unsigned math path)");
    mc_real_t v = MC_FP_C(500);
    mc_real_t r = mc_fp_div(v, MC_FP_C(1));
    CHECK(MC_REAL_TO_INT(r) == 500);
    PASS(); return 0;
}

static int test_fp_div_negative() {
    TEST("fp_div: -100 / 2");
    mc_real_t r = mc_fp_div(MC_FP_C(-100), MC_FP_C(2));
    CHECK(MC_REAL_TO_INT(r) == -50);
    PASS(); return 0;
}

static int test_fp_div_by_large() {
    TEST("fp_div: 1 / 100");
    mc_real_t r = mc_fp_div(MC_FP_C(1), MC_FP_C(100));
    CHECK(r >= 0);
    PASS(); return 0;
}

static int test_fp_mul_negative() {
    TEST("fp_mul: -30 * 4");
    mc_real_t r = mc_fp_mul(MC_FP_C(-30), MC_FP_C(4));
    CHECK(MC_REAL_TO_INT(r) == -120);
    PASS(); return 0;
}

static int test_fp_mul_negative_both() {
    TEST("fp_mul: -30 * -4");
    mc_real_t r = mc_fp_mul(MC_FP_C(-30), MC_FP_C(-4));
    CHECK(MC_REAL_TO_INT(r) == 120);
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
    mc_real_t displacement = MC_FP_C(400) - MC_FP_C(0);
    mc_real_t force_mag = mc_fp_mul(MC_FP_C(120), displacement);
    int force_int = MC_REAL_TO_INT(force_mag);
    if (force_int < 0) {
        printf("FORCE NEGATIVE: 120*400 should be >=0 but got %d ", force_int);
        CHECK(0 && "fp_mul returned negative for positive operands!");
    }

    mc_real_t x = MC_FP_C(400);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = {
        MC_FP_C(120),
        MC_FP_C(12),
        MC_FP_C(1)
    };
    mc_real_t target = MC_FP_C(0);
    mc_real_t dt = MC_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Run many frames, check for convergence or near zero */
    for (int i = 0; i < 300; i++) {
        mc_spring_step(&x, &s, &p, target, dt);
        /* After a few frames, x should still be reasonable */
        if (i > 10) {
            int xi = MC_REAL_TO_INT(x);
            /* x should not exceed sane bounds */
            if (xi < -200 || xi > 500) {
                printf("\n  FAIL: frame=%d x=%d (out of range)\n", i, xi);
                return 1;
            }
        }
    }

    /* After 300 frames (5s), should be near 0 */
    mc_real_t error = x - target;
    if (error < 0) error = -error;
    int ei = MC_REAL_TO_INT(error);
    printf("final error=%d ", ei);
    CHECK(ei < 10);
    PASS(); return 0;
}

/*
 * Full simulation: mc_animate_t spring mode, click to retarget
 * mimicking the exact animate_value.c pattern.
 */
static int test_spring_anim_retarget() {
    TEST("mc_animate spring: init at 400, retarget to click positions");

    mc_animate_t anim_x;
    mc_animate_init_spring(&anim_x, MC_FP_C(400), MC_FP_C(400));
    anim_x.config.spring.stiffness = MC_FP_C(120);
    anim_x.config.spring.damping = MC_FP_C(12);

    mc_real_t dt = MC_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Let it settle for 120 frames */
    for (int i = 0; i < 120; i++) {
        mc_animate_update(&anim_x, dt);
    }

    /* Click 1: target = 100 (left side) */
    mc_real_t targets[] = {
        MC_FP_C(100),
        MC_FP_C(600),
        MC_FP_C(50),
        MC_FP_C(700),
        MC_FP_C(10),
    };

    for (int click = 0; click < 5; click++) {
        /* Retarget: from=current, to=mouse, velocity=0, elapsed=0 */
        anim_x.to = targets[click];
        anim_x.from = anim_x.current;
        anim_x.velocity = 0;
        anim_x.elapsed = 0;

        /* Run for 120 frames, checking x never goes wildly negative */
        for (int i = 0; i < 120; i++) {
            mc_animate_update(&anim_x, dt);
            int xi = MC_REAL_TO_INT(anim_x.current);
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

    mc_animate_t anim_x;
    mc_animate_init_spring(&anim_x, MC_FP_C(400), MC_FP_C(400));
    anim_x.config.spring.stiffness = MC_FP_C(120);
    anim_x.config.spring.damping = MC_FP_C(12);

    mc_real_t dt = MC_REAL_FROM_FLOAT(1.0f / 60.0f);

    /* Settle */
    for (int i = 0; i < 60; i++) mc_animate_update(&anim_x, dt);

    /* Click at x=5 (near edge) */
    anim_x.to = MC_REAL_FROM_INT(5);
    anim_x.from = anim_x.current;
    anim_x.velocity = 0;
    anim_x.elapsed = 0;

    for (int i = 0; i < 300; i++) {
        mc_animate_update(&anim_x, dt);
        int xi = MC_REAL_TO_INT(anim_x.current);
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
