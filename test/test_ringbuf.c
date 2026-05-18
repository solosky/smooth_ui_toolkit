#include <stdio.h>
#include "mc_ringbuf.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_push_pop() {
    TEST("push 3, pop 3");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 5) == MC_OK);
    int v;
    for (int i = 0; i < 3; i++) { v = i * 10; CHECK(mc_ringbuf_push(&rb, &v) == MC_OK); }
    CHECK(mc_ringbuf_count(&rb) == 3);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_OK); CHECK(v == 0);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_OK); CHECK(v == 10);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_OK); CHECK(v == 20);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_ERR_EMPTY);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_overwrite() {
    TEST("overwrite: push 7 into cap 5");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 5) == MC_OK);
    rb.overwrite = 1;
    int v;
    for (int i = 0; i < 7; i++) { v = i; CHECK(mc_ringbuf_push(&rb, &v) == MC_OK); }
    CHECK(mc_ringbuf_count(&rb) == 5);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_OK); CHECK(v == 2);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_push_pop();
    failed += test_overwrite();
    printf("RingBuf: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
