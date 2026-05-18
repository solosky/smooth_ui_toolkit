#include <stdio.h>
#include "mc_ringbuf.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_push_pop() {
    TEST("push 3 ints, pop 3 ints");
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
    TEST("overwrite: push 7 into cap 5, oldest 2 lost");
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

static int test_peek() {
    TEST("peek valid and invalid indices");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 5) == MC_OK);
    int v;
    for (int i = 0; i < 3; i++) { v = i * 10; mc_ringbuf_push(&rb, &v); }
    CHECK(mc_ringbuf_peek(&rb, 0, &v) == MC_OK); CHECK(v == 0);
    CHECK(mc_ringbuf_peek(&rb, 1, &v) == MC_OK); CHECK(v == 10);
    CHECK(mc_ringbuf_peek(&rb, 2, &v) == MC_OK); CHECK(v == 20);
    CHECK(mc_ringbuf_peek(&rb, 3, &v) == MC_ERR_INVAL);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_clear() {
    TEST("clear resets ring buffer to empty");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 5) == MC_OK);
    int v = 42;
    mc_ringbuf_push(&rb, &v);
    mc_ringbuf_clear(&rb);
    CHECK(mc_ringbuf_count(&rb) == 0);
    CHECK(mc_ringbuf_pop(&rb, &v) == MC_ERR_EMPTY);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_wrap_around() {
    TEST("wrap around: fill, pop, push cycle");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 3) == MC_OK);
    int v;
    for (int i = 0; i < 3; i++) { v = i; mc_ringbuf_push(&rb, &v); }
    mc_ringbuf_pop(&rb, &v); CHECK(v == 0);
    mc_ringbuf_pop(&rb, &v); CHECK(v == 1);
    v = 100; mc_ringbuf_push(&rb, &v);
    v = 200; mc_ringbuf_push(&rb, &v);
    CHECK(mc_ringbuf_count(&rb) == 3);
    mc_ringbuf_pop(&rb, &v); CHECK(v == 2);
    mc_ringbuf_pop(&rb, &v); CHECK(v == 100);
    mc_ringbuf_pop(&rb, &v); CHECK(v == 200);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_no_overwrite_returns_full() {
    TEST("push to full without overwrite returns MC_ERR_FULL");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 2) == MC_OK);
    int v;
    for (int i = 0; i < 2; i++) { v = i; CHECK(mc_ringbuf_push(&rb, &v) == MC_OK); }
    v = 99; CHECK(mc_ringbuf_push(&rb, &v) == MC_ERR_FULL);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_struct_elements() {
    TEST("push/pop struct elements");
    typedef struct { int id; float val; } item_t;
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(item_t), 4) == MC_OK);
    item_t items[] = { {1, 1.5f}, {2, 2.5f}, {3, 3.5f} };
    for (int i = 0; i < 3; i++)
        CHECK(mc_ringbuf_push(&rb, &items[i]) == MC_OK);
    CHECK(mc_ringbuf_count(&rb) == 3);
    item_t out;
    mc_ringbuf_pop(&rb, &out);
    CHECK(out.id == 1);
    mc_ringbuf_pop(&rb, &out);
    CHECK(out.id == 2);
    mc_ringbuf_pop(&rb, &out);
    CHECK(out.id == 3);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_byte_elements() {
    TEST("push/pop single bytes");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, 1, 4) == MC_OK);
    uint8_t bytes[] = { 0xAB, 0xCD, 0xEF };
    for (int i = 0; i < 3; i++)
        CHECK(mc_ringbuf_push(&rb, &bytes[i]) == MC_OK);
    uint8_t b;
    mc_ringbuf_pop(&rb, &b); CHECK(b == 0xAB);
    mc_ringbuf_pop(&rb, &b); CHECK(b == 0xCD);
    mc_ringbuf_pop(&rb, &b); CHECK(b == 0xEF);
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_push_pop_cycle() {
    TEST("multiple push/pop cycles maintain order");
    mc_ringbuf_t rb;
    CHECK(mc_ringbuf_init(&rb, sizeof(int), 3) == MC_OK);
    int v;
    for (int cycle = 0; cycle < 5; cycle++) {
        v = cycle * 10; mc_ringbuf_push(&rb, &v);
        if (mc_ringbuf_count(&rb) == 3) {
            mc_ringbuf_pop(&rb, &v);
            CHECK(v == cycle * 10 - 20);
        }
    }
    mc_ringbuf_deinit(&rb);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_push_pop();
    failed += test_overwrite();
    failed += test_peek();
    failed += test_clear();
    failed += test_wrap_around();
    failed += test_no_overwrite_returns_full();
    failed += test_struct_elements();
    failed += test_byte_elements();
    failed += test_push_pop_cycle();
    printf("RingBuf: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
