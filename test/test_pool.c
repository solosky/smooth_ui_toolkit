#include <stdio.h>
#include "mc_pool.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

typedef struct { int value; } test_obj_t;
static void ctor(void* obj, void* ctx) { ((test_obj_t*)obj)->value = *(int*)ctx; }
static void foreach_counter(void* obj, void* ctx) { (*(int*)ctx)++; }

static int test_acquire_release() {
    TEST("acquire 2, release 1, acquire again");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(test_obj_t), 5) == MC_OK);
    int v42 = 42, v99 = 99;
    test_obj_t* a = (test_obj_t*)mc_pool_acquire(&pool, ctor, &v42);
    test_obj_t* b = (test_obj_t*)mc_pool_acquire(&pool, ctor, &v99);
    CHECK(a != NULL && b != NULL);
    CHECK(a->value == 42);
    CHECK(b->value == 99);
    CHECK(mc_pool_available(&pool) == 3);
    mc_pool_release(&pool, a);
    CHECK(mc_pool_available(&pool) == 4);
    int v7 = 7;
    test_obj_t* c = (test_obj_t*)mc_pool_acquire(&pool, ctor, &v7);
    CHECK(c != NULL && c->value == 7);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_full_pool() {
    TEST("acquire all slots, next returns NULL");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(test_obj_t), 3) == MC_OK);
    int v = 0;
    CHECK(mc_pool_acquire(&pool, ctor, &v) != NULL);
    CHECK(mc_pool_acquire(&pool, ctor, &v) != NULL);
    CHECK(mc_pool_acquire(&pool, ctor, &v) != NULL);
    CHECK(mc_pool_acquire(&pool, ctor, &v) == NULL);
    CHECK(mc_pool_available(&pool) == 0);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_foreach() {
    TEST("acquire 2, foreach counts 2");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 5) == MC_OK);
    int count = 0;
    int v1 = 10, v2 = 20;
    mc_pool_acquire(&pool, ctor, &v1);
    mc_pool_acquire(&pool, ctor, &v2);
    mc_pool_foreach(&pool, foreach_counter, &count);
    CHECK(count == 2);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_null_ctor() {
    TEST("acquire with NULL constructor returns valid object");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 3) == MC_OK);
    void* obj = mc_pool_acquire(&pool, NULL, NULL);
    CHECK(obj != NULL);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_release_all() {
    TEST("acquire all, release all, acquire again");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 3) == MC_OK);
    void* objs[3];
    int v = 0;
    for (int i = 0; i < 3; i++) objs[i] = mc_pool_acquire(&pool, ctor, &v);
    CHECK(mc_pool_available(&pool) == 0);
    for (int i = 0; i < 3; i++) mc_pool_release(&pool, objs[i]);
    CHECK(mc_pool_available(&pool) == 3);
    CHECK(mc_pool_acquire(&pool, ctor, &v) != NULL);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_zero_capacity() {
    TEST("zero capacity init returns MC_OK, acquire returns NULL");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 0) == MC_OK);
    void* obj = mc_pool_acquire(&pool, NULL, NULL);
    CHECK(obj == NULL);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

static int test_lifo_order() {
    TEST("LIFO order: most recently released is reused first");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 3) == MC_OK);
    int v = 0;
    void* a = mc_pool_acquire(&pool, NULL, NULL);
    void* b = mc_pool_acquire(&pool, NULL, NULL);
    mc_pool_release(&pool, b);
    mc_pool_release(&pool, a);
    void* c = mc_pool_acquire(&pool, NULL, NULL);
    CHECK(c == a);
    PASS(); return 0;
}

static int test_release_invalid() {
    TEST("release pointer not from pool does nothing (no crash)");
    mc_pool_t pool;
    CHECK(mc_pool_init(&pool, sizeof(int), 3) == MC_OK);
    int stack_var = 42;
    mc_pool_release(&pool, &stack_var);
    int v = 0;
    void* obj = mc_pool_acquire(&pool, NULL, NULL);
    CHECK(obj != NULL);
    mc_pool_deinit(&pool);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_acquire_release();
    failed += test_full_pool();
    failed += test_foreach();
    failed += test_null_ctor();
    failed += test_release_all();
    failed += test_zero_capacity();
    failed += test_lifo_order();
    failed += test_release_invalid();
    printf("Pool: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
