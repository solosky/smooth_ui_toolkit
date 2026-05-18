#include <stdio.h>
#include "mc_hal.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n); } while(0)
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static uint32_t mock_tick;
static uint32_t mock_delay_ms;
static uint32_t my_tick(void) { return mock_tick; }
static void my_delay(uint32_t ms) { mock_delay_ms = ms; }

static int test_default_tick() {
    TEST("default get_tick returns 0");
    CHECK(mc_get_tick() == 0);
    PASS(); return 0;
}

static int test_set_tick_callback() {
    TEST("set tick callback returns mock value");
    mc_hal_set_tick_callback(my_tick);
    mock_tick = 12345;
    CHECK(mc_get_tick() == 12345);
    PASS(); return 0;
}

static int test_default_delay() {
    TEST("default delay does nothing (no crash)");
    mc_delay(100);
    PASS(); return 0;
}

static int test_set_delay_callback() {
    TEST("set delay callback forwards ms");
    mc_hal_set_delay_callback(my_delay);
    mock_delay_ms = 0;
    mc_delay(250);
    CHECK(mock_delay_ms == 250);
    PASS(); return 0;
}

static int test_tick_increment() {
    TEST("tick callback returns updated value each call");
    mock_tick = 0;
    CHECK(mc_get_tick() == 0);
    mock_tick = 999;
    CHECK(mc_get_tick() == 999);
    PASS(); return 0;
}

static int test_null_callbacks() {
    TEST("null callbacks after clear don't crash");
    mc_hal_set_tick_callback(NULL);
    mc_hal_set_delay_callback(NULL);
    CHECK(mc_get_tick() == 0);
    mc_delay(100);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_default_tick();
    failed += test_set_tick_callback();
    failed += test_default_delay();
    failed += test_set_delay_callback();
    failed += test_tick_increment();
    failed += test_null_callbacks();
    printf("HAL: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
