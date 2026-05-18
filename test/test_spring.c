#include <stdio.h>
#include "mc_spring.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(name) do { tests_run++; printf("  %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c)   do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_spring_converges() {
    TEST("spring converges to target (stiff=100, damp=10)");
    mc_real_t x = MC_FP_C(0);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(100), MC_FP_C(10), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 1000; i++)
        mc_spring_step(&x, &s, &p, target, dt);
    mc_real_t error = x - target;
    if (error < 0) error = -error;
    CHECK(error < MC_FP_C(0.01f));
    PASS(); return 0;
}

static int test_underdamped() {
    TEST("underdamped: low damping should overshoot then settle");
    mc_real_t x = MC_FP_C(0);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(100), MC_FP_C(3), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    int overshot = 0;
    for (int i = 0; i < 500; i++) {
        mc_spring_step(&x, &s, &p, target, dt);
        if (x > target + MC_FP_C(0.1f)) overshot = 1;
    }
    CHECK(overshot == 1);
    mc_real_t error = x - target;
    if (error < 0) error = -error;
    CHECK(error < MC_FP_C(0.1f));
    PASS(); return 0;
}

static int test_critical_damping() {
    TEST("critically damped: no overshoot, approach target");
    mc_real_t x = MC_FP_C(0);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(200), MC_FP_C(28), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 200; i++) {
        mc_spring_step(&x, &s, &p, target, dt);
        CHECK(x <= target + MC_FP_C(0.1f));
    }
    PASS(); return 0;
}

static int test_overdamped() {
    TEST("overdamped: slow approach, no overshoot");
    mc_real_t x = MC_FP_C(0);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(100), MC_FP_C(50), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 200; i++) {
        mc_spring_step(&x, &s, &p, target, dt);
        CHECK(x <= target + MC_FP_C(0.1f));
    }
    PASS(); return 0;
}

static int test_zero_stiffness() {
    TEST("zero stiffness: no spring force, x should not move to target");
    mc_real_t x = MC_FP_C(0);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(0), MC_FP_C(10), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 100; i++)
        mc_spring_step(&x, &s, &p, target, dt);
    CHECK(x < MC_FP_C(1));
    PASS(); return 0;
}

static int test_at_target() {
    TEST("already at target: x should stay at target");
    mc_real_t x = MC_FP_C(10);
    mc_spring_state_t s = { 0 };
    mc_spring_params_t p = { MC_FP_C(100), MC_FP_C(10), MC_FP_C(1) };
    mc_real_t target = MC_FP_C(10);
    mc_real_t dt = MC_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 10; i++)
        mc_spring_step(&x, &s, &p, target, dt);
    CHECK(x == MC_FP_C(10));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_spring_converges();
    failed += test_underdamped();
    failed += test_critical_damping();
    failed += test_overdamped();
    failed += test_zero_stiffness();
    failed += test_at_target();
    printf("Spring: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
