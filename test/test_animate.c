#include <stdio.h>
#include "mc_animate.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_easing_anim_completes() {
    TEST("easing anim from 0 to 10 over 1s");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    bool done = mc_animate_update(&a, MC_FP_C(1));
    CHECK(done == true);
    CHECK(MC_REAL_TO_INT(a.current) == 10);
    PASS(); return 0;
}

static int test_easing_anim_partial() {
    TEST("easing anim at t=0.5 linear");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.config.easing.easing = mc_ease_linear;
    bool done = mc_animate_update(&a, MC_FP_C(0.5f));
    CHECK(done == false);
    CHECK(MC_REAL_TO_INT(a.current) == 5);
    PASS(); return 0;
}

static int test_spring_anim() {
    TEST("spring anim from 0 to 10");
    mc_animate_t a;
    mc_animate_init_spring(&a, MC_FP_C(0), MC_FP_C(10));
    mc_animate_update(&a, MC_FP_C(1.0f / 60.0f));
    CHECK(a.current > 0);
    CHECK(a.current < MC_FP_C(10));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_easing_anim_completes();
    failed += test_easing_anim_partial();
    failed += test_spring_anim();
    printf("Animate: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
