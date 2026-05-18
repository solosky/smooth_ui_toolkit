#include <stdio.h>
#include "mc_animate.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int update_count = 0;
static int complete_count = 0;
static mc_real_t last_value = 0;
static void on_update(void* ctx, mc_real_t value) { update_count++; last_value = value; }
static void on_complete(void* ctx) { complete_count++; }

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
    TEST("easing anim at t=0.5 linear = 5");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.config.easing.easing = mc_ease_linear;
    bool done = mc_animate_update(&a, MC_FP_C(0.5f));
    CHECK(done == false);
    CHECK(MC_REAL_TO_INT(a.current) == 5);
    PASS(); return 0;
}

static int test_easing_with_repeat() {
    TEST("easing anim with repeat=2");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.config.easing.easing = mc_ease_linear;
    a.repeat = 2;
    bool done = mc_animate_update(&a, MC_FP_C(1));
    CHECK(done == false);
    CHECK(MC_REAL_TO_INT(a.current) == 10);
    done = mc_animate_update(&a, MC_FP_C(1));
    CHECK(done == true);
    PASS(); return 0;
}

static int test_easing_never_completes_with_infinite_repeat() {
    TEST("easing anim with repeat=255 (infinite) never completes");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.repeat = 255;
    for (int i = 0; i < 10; i++) {
        bool done = mc_animate_update(&a, MC_FP_C(1));
        CHECK(done == false);
    }
    PASS(); return 0;
}

static int test_easing_with_delay() {
    TEST("easing anim with delay=0.5s");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.delay = MC_FP_C(0.5f);
    a.config.easing.easing = mc_ease_linear;
    bool done = mc_animate_update(&a, MC_FP_C(0.4f));
    CHECK(done == false);
    CHECK(MC_REAL_TO_INT(a.current) == 0);
    done = mc_animate_update(&a, MC_FP_C(0.2f));
    CHECK(done == false);
    CHECK(a.current > MC_FP_C(0));
    PASS(); return 0;
}

static int test_easing_callbacks() {
    TEST("easing anim on_update and on_complete callbacks");
    update_count = 0;
    complete_count = 0;
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    a.on_update = on_update;
    a.on_complete = on_complete;
    bool done = mc_animate_update(&a, MC_FP_C(1));
    CHECK(done == true);
    CHECK(update_count >= 1);
    CHECK(complete_count == 1);
    PASS(); return 0;
}

static int test_spring_anim() {
    TEST("spring anim from 0 to 10, moves toward target");
    mc_animate_t a;
    mc_animate_init_spring(&a, MC_FP_C(0), MC_FP_C(10));
    mc_animate_update(&a, MC_FP_C(1.0f / 60.0f));
    CHECK(a.current > 0);
    CHECK(a.current < MC_FP_C(10));
    PASS(); return 0;
}

static int test_spring_callbacks() {
    TEST("spring anim calls on_update each step");
    update_count = 0;
    mc_animate_t a;
    mc_animate_init_spring(&a, MC_FP_C(0), MC_FP_C(10));
    a.on_update = on_update;
    mc_animate_update(&a, MC_FP_C(1.0f / 60.0f));
    CHECK(update_count == 1);
    PASS(); return 0;
}

static int test_reset() {
    TEST("reset puts anim back to from value");
    mc_animate_t a;
    mc_animate_init_easing(&a, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
    mc_animate_update(&a, MC_FP_C(1));
    CHECK(MC_REAL_TO_INT(a.current) == 10);
    mc_animate_reset(&a);
    CHECK(a.current == MC_FP_C(0));
    CHECK(a.elapsed == 0);
    PASS(); return 0;
}

static int test_vec2_animate_easing() {
    TEST("vec2 animate easing moves from a to b");
    mc_vec2_animate_t va;
    mc_vec2_animate_init_easing(&va,
        mc_vec2(MC_FP_C(0), MC_FP_C(0)),
        mc_vec2(MC_FP_C(100), MC_FP_C(200)),
        MC_FP_C(1));
    bool done = mc_vec2_animate_update(&va, MC_FP_C(1));
    CHECK(done == true);
    CHECK(MC_REAL_TO_INT(va.current.x) == 100);
    CHECK(MC_REAL_TO_INT(va.current.y) == 200);
    PASS(); return 0;
}

static int test_vec2_animate_move_to() {
    TEST("vec2 animate move_to retargets");
    mc_vec2_animate_t va;
    mc_vec2_animate_init_spring(&va,
        mc_vec2(MC_FP_C(0), MC_FP_C(0)),
        mc_vec2(MC_FP_C(100), MC_FP_C(100)));
    mc_vec2_animate_move_to(&va, mc_vec2(MC_FP_C(200), MC_FP_C(50)));
    CHECK(va.to.x == MC_FP_C(200));
    CHECK(va.to.y == MC_FP_C(50));
    PASS(); return 0;
}

static int test_vec4_animate_easing() {
    TEST("vec4 animate easing moves from a to b");
    mc_vec4_animate_t va;
    mc_vec4_animate_init_easing(&va,
        mc_vec4(MC_FP_C(0), MC_FP_C(0), MC_FP_C(0), MC_FP_C(0)),
        mc_vec4(MC_FP_C(10), MC_FP_C(20), MC_FP_C(30), MC_FP_C(40)),
        MC_FP_C(1));
    bool done = mc_vec4_animate_update(&va, MC_FP_C(1));
    CHECK(done == true);
    CHECK(MC_REAL_TO_INT(va.current.x) == 10);
    CHECK(MC_REAL_TO_INT(va.current.w) == 40);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_easing_anim_completes();
    failed += test_easing_anim_partial();
    failed += test_easing_with_repeat();
    failed += test_easing_never_completes_with_infinite_repeat();
    failed += test_easing_with_delay();
    failed += test_easing_callbacks();
    failed += test_spring_anim();
    failed += test_spring_callbacks();
    failed += test_reset();
    failed += test_vec2_animate_easing();
    failed += test_vec2_animate_move_to();
    failed += test_vec4_animate_easing();
    printf("Animate: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
