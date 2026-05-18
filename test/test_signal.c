#include <stdio.h>
#include "mc_signal.h"

static int calls = 0;
static void handler1(void* ctx, void* args) { calls++; if (args) *(int*)args += 1; }
static void handler2(void* ctx, void* args) { calls++; if (args) *(int*)args += 2; }
static void handler3(void* ctx, void* args) { calls++; if (args) *(int*)args += 3; }

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_signal_emit() {
    TEST("connect 2 handlers, emit, both fire");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    CHECK(mc_signal_connect(&sig, handler1, NULL) == MC_OK);
    CHECK(mc_signal_connect(&sig, handler2, NULL) == MC_OK);
    calls = 0;
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 3);
    CHECK(calls == 2);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_signal_disconnect() {
    TEST("connect 2, disconnect 1, emit remaining");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    mc_signal_connect(&sig, handler1, NULL);
    mc_signal_connect(&sig, handler2, NULL);
    mc_signal_disconnect(&sig, handler1, NULL);
    calls = 0;
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 2);
    CHECK(calls == 1);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_full_capacity() {
    TEST("fill capacity, next connect returns MC_ERR_FULL");
    mc_signal_t sig;
    mc_signal_init(&sig, 2);
    CHECK(mc_signal_connect(&sig, handler1, NULL) == MC_OK);
    CHECK(mc_signal_connect(&sig, handler2, NULL) == MC_OK);
    CHECK(mc_signal_connect(&sig, handler3, NULL) == MC_ERR_FULL);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_double_disconnect() {
    TEST("disconnect twice (second is no-op)");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    mc_signal_connect(&sig, handler1, NULL);
    mc_signal_connect(&sig, handler2, NULL);
    mc_signal_disconnect(&sig, handler1, NULL);
    mc_signal_disconnect(&sig, handler1, NULL);
    calls = 0;
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 2);
    CHECK(calls == 1);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_null_emit() {
    TEST("emit with no connected slots is no-op");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 0);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_many_slots() {
    TEST("connect 10 handlers, emit once, all fire");
    mc_signal_t sig;
    mc_signal_init(&sig, 10);
    for (int i = 0; i < 10; i++)
        mc_signal_connect(&sig, handler1, NULL);
    calls = 0;
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 10);
    CHECK(calls == 10);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_emit_null_args() {
    TEST("emit with NULL args does not crash");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    mc_signal_connect(&sig, handler1, NULL);
    calls = 0;
    mc_signal_emit(&sig, NULL);
    CHECK(calls == 1);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_connect_with_context() {
    TEST("connect with ctx, disconnect matches both cb and ctx");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    int ctx_a = 1, ctx_b = 2;
    mc_signal_connect(&sig, handler1, &ctx_a);
    mc_signal_connect(&sig, handler1, &ctx_b);
    mc_signal_disconnect(&sig, handler1, &ctx_a);
    calls = 0;
    mc_signal_emit(&sig, NULL);
    CHECK(calls == 1);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_reinit_after_deinit() {
    TEST("reinit after deinit works cleanly");
    mc_signal_t sig;
    mc_signal_init(&sig, 5);
    mc_signal_connect(&sig, handler1, NULL);
    mc_signal_deinit(&sig);
    mc_signal_init(&sig, 3);
    CHECK(mc_signal_connect(&sig, handler2, NULL) == MC_OK);
    int val = 0;
    mc_signal_emit(&sig, &val);
    CHECK(val == 2);
    mc_signal_deinit(&sig);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_signal_emit();
    failed += test_signal_disconnect();
    failed += test_full_capacity();
    failed += test_double_disconnect();
    failed += test_null_emit();
    failed += test_many_slots();
    failed += test_emit_null_args();
    failed += test_connect_with_context();
    failed += test_reinit_after_deinit();
    printf("Signal: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
