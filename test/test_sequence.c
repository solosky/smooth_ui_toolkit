#include <stdio.h>
#include "sut_sequence.h"

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
    sut_sequence_t seq;
    CHECK(sut_sequence_init(&seq, 10) == SUT_OK);
    seq.on_step = on_step;
    seq.on_complete = on_complete;
    CHECK(sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(10), SUT_FP_C(0.5f), sut_ease_linear }) == SUT_OK);
    CHECK(sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(20), SUT_FP_C(0.5f), sut_ease_linear }) == SUT_OK);
    CHECK(sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(30), SUT_FP_C(0.5f), sut_ease_linear }) == SUT_OK);
    sut_sequence_play(&seq);
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.5f)) == false);
    CHECK(step_index == 0);
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.5f)) == false);
    CHECK(step_index == 1);
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.6f)) == true);
    CHECK(step_index == 2);
    CHECK(complete_called == 1);
    sut_sequence_deinit(&seq);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_seq_3_steps();
    printf("Sequence: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
