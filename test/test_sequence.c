#include <stdio.h>
#include "mc_sequence.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int step_index = 0;
static int complete_called = 0;
static void on_step(void* ctx, int index) { step_index = index; }
static void on_complete(void* ctx) { complete_called = 1; }

static int test_seq_3_steps() {
    TEST("3 steps, each 0.5s");
    mc_sequence_t seq;
    CHECK(mc_sequence_init(&seq, 10) == MC_OK);
    seq.on_step = on_step;
    seq.on_complete = on_complete;
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(10), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(20), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(30), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    mc_sequence_play(&seq);
    CHECK(mc_sequence_update(&seq, MC_FP_C(0.5f)) == false);
    CHECK(step_index == 0);
    CHECK(mc_sequence_update(&seq, MC_FP_C(0.5f)) == false);
    CHECK(step_index == 1);
    CHECK(mc_sequence_update(&seq, MC_FP_C(0.6f)) == true);
    CHECK(step_index == 2);
    CHECK(complete_called == 1);
    mc_sequence_deinit(&seq);
    PASS(); return 0;
}

static int test_empty_sequence() {
    TEST("empty sequence returns true immediately");
    mc_sequence_t seq;
    CHECK(mc_sequence_init(&seq, 10) == MC_OK);
    bool done = mc_sequence_update(&seq, MC_FP_C(1));
    CHECK(done == true);
    mc_sequence_deinit(&seq);
    PASS(); return 0;
}

static int test_single_step() {
    TEST("single step sequence");
    mc_sequence_t seq;
    CHECK(mc_sequence_init(&seq, 10) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(50), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    mc_sequence_play(&seq);
    CHECK(mc_sequence_update(&seq, MC_FP_C(0.6f)) == true);
    mc_sequence_deinit(&seq);
    PASS(); return 0;
}

static int test_clear() {
    TEST("clear sequence, push again, verify works");
    mc_sequence_t seq;
    CHECK(mc_sequence_init(&seq, 10) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(10), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    mc_sequence_clear(&seq);
    CHECK(seq.count == 0);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(20), MC_FP_C(0.5f), mc_ease_linear }) == MC_OK);
    mc_sequence_play(&seq);
    bool done = mc_sequence_update(&seq, MC_FP_C(0.6f));
    CHECK(done == true);
    mc_sequence_deinit(&seq);
    PASS(); return 0;
}

static int test_full_capacity() {
    TEST("push to capacity, next push returns MC_ERR_FULL");
    mc_sequence_t seq;
    CHECK(mc_sequence_init(&seq, 2) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(10), MC_FP_C(1), mc_ease_linear }) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(20), MC_FP_C(1), mc_ease_linear }) == MC_OK);
    CHECK(mc_sequence_push(&seq, (mc_animate_step_t){ MC_FP_C(30), MC_FP_C(1), mc_ease_linear }) == MC_ERR_FULL);
    mc_sequence_deinit(&seq);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_seq_3_steps();
    failed += test_empty_sequence();
    failed += test_single_step();
    failed += test_clear();
    failed += test_full_capacity();
    printf("Sequence: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
