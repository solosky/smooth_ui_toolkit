#include <stdio.h>
#include <string.h>
#include "mc_allocator.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int alloc_count;
static int free_count;
static void* tracking_malloc(size_t size, void* ctx) {
    alloc_count++;
    void* p = malloc(size);
    return p;
}
static void tracking_free(void* ptr, void* ctx) {
    free_count++;
    free(ptr);
}

static int test_default_alloc_free() {
    TEST("default allocator malloc/free works");
    int* p = mc_malloc(sizeof(int));
    CHECK(p != NULL);
    *p = 42;
    CHECK(*p == 42);
    mc_free(p);
    PASS(); return 0;
}

static int test_custom_allocator() {
    TEST("custom allocator set and called");
    alloc_count = 0;
    free_count = 0;
    mc_allocator_t my_alloc = { tracking_malloc, tracking_free, NULL };
    mc_allocator_set(&my_alloc);
    int* p = mc_malloc(sizeof(int) * 10);
    CHECK(p != NULL);
    CHECK(alloc_count == 1);
    mc_free(p);
    CHECK(free_count == 1);
    PASS(); return 0;
}

static int test_null_allocator_set() {
    TEST("mc_allocator_set(NULL) does nothing (no crash)");
    mc_allocator_set(NULL);
    int* p = mc_malloc(sizeof(int));
    CHECK(p != NULL);
    mc_free(p);
    PASS(); return 0;
}

static int test_alloc_zero_size() {
    TEST("mc_malloc(0) returns valid or NULL (no crash)");
    void* p = mc_malloc(0);
    mc_free(p);
    PASS(); return 0;
}

static int test_alloc_large() {
    TEST("mc_malloc large block works");
    void* p = mc_malloc(1024 * 64);
    CHECK(p != NULL);
    memset(p, 0xAB, 1024 * 64);
    mc_free(p);
    PASS(); return 0;
}

static int test_free_null() {
    TEST("mc_free(NULL) does nothing (no crash)");
    mc_free(NULL);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_default_alloc_free();
    failed += test_custom_allocator();
    failed += test_null_allocator_set();
    failed += test_alloc_zero_size();
    failed += test_alloc_large();
    failed += test_free_null();
    printf("Allocator: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
