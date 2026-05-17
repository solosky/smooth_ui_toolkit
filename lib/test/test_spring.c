#include <stdio.h>
#include "sut_spring.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(name) do { tests_run++; printf("  %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_spring_converges() {
    TEST("spring converges to target");
    sut_real_t x = SUT_FP_C(0);
    sut_spring_state_t s = { 0 };
    sut_spring_params_t p = { SUT_FP_C(100), SUT_FP_C(10), SUT_FP_C(1) };
    sut_real_t target = SUT_FP_C(10);
    sut_real_t dt = SUT_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 1000; i++)
        sut_spring_step(&x, &s, &p, target, dt);
    sut_real_t error = x - target;
    if (error < 0) error = -error;
    CHECK(error < SUT_FP_C(0.01f));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_spring_converges();
    printf("Spring: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
