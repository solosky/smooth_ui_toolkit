#include <stdio.h>
#include "mc_signal.h"

static int calls = 0;
static void handler1(void* ctx, void* args) { calls++; *(int*)args += 1; }
static void handler2(void* ctx, void* args) { calls++; *(int*)args += 2; }

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_signal_emit() {
    TEST("connect 2, emit");
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
    TEST("connect, disconnect, emit");
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

int main() {
    int failed = 0;
    failed += test_signal_emit();
    failed += test_signal_disconnect();
    printf("Signal: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
