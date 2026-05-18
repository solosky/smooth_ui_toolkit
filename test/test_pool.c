#include <stdio.h>
#include "sut_pool.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

typedef struct { int value; } test_obj_t;
static void ctor(void* obj, void* ctx) { ((test_obj_t*)obj)->value = *(int*)ctx; }

static int test_acquire_release() {
    TEST("acquire 2, release 1, acquire again");
    sut_pool_t pool;
    CHECK(sut_pool_init(&pool, sizeof(test_obj_t), 5) == SUT_OK);
    int v42 = 42, v99 = 99;
    test_obj_t* a = (test_obj_t*)sut_pool_acquire(&pool, ctor, &v42);
    test_obj_t* b = (test_obj_t*)sut_pool_acquire(&pool, ctor, &v99);
    CHECK(a != NULL && b != NULL);
    CHECK(a->value == 42);
    CHECK(b->value == 99);
    CHECK(sut_pool_available(&pool) == 3);
    sut_pool_release(&pool, a);
    CHECK(sut_pool_available(&pool) == 4);
    int v7 = 7;
    test_obj_t* c = (test_obj_t*)sut_pool_acquire(&pool, ctor, &v7);
    CHECK(c != NULL && c->value == 7);
    sut_pool_deinit(&pool);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_acquire_release();
    printf("Pool: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
