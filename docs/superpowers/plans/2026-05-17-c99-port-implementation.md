# Smooth UI Toolkit — C99 Port Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Port the C++17 animation/UI toolkit to idiomatic C99 with Q16.16 fixed-point math, targeting embedded MCUs without FPU.

**Architecture:** Header-only type system (`sut_real_t` switches between float/fixed-point at compile time), static library `libsut.a`. Modules: math → easing/spring → animate/sequence → color/HAL → tools (ringbuf, signal, pool). Each module has its own header, implementation, and test file.

**Tech Stack:** C99, CMake, CTest (simple assert-based test runners)

---

## File Structure

```
lib/
  CMakeLists.txt
  include/
    sut_config.h          — SUT_USE_FLOAT user switch
    sut_types.h           — sut_real_t, SUT_FP_SCALE, conversion macros
    sut_allocator.h       — sut_allocator_t, sut_allocator_set, sut_malloc/sut_free
    sut_errors.h          — SUT_OK, SUT_ERR_* codes
    sut_math.h            — clamp, lerp, map, fp_mul/div/sqrt, vec2, vec4, rect
    sut_easing.h          — easing_fn_t, LUT externs, function decls
    sut_spring.h          — spring_params_t, spring_state_t, sut_spring_step
    sut_color.h           — rgb_t, rgba_t, hex/hsv/blending
    sut_animate.h         — animate_t, vec2_animate_t, vec4_animate_t
    sut_sequence.h        — animate_step_t, sequence_t
    sut_hal.h             — get_tick_t, delay_t, hal_set callbacks
    sut_ringbuf.h         — ringbuf_t
    sut_signal.h          — slot_t, signal_t
    sut_pool.h            — pool_t
    sut.h                 — umbrella, includes all above
  src/
    sut_math.c
    sut_easing.c
    sut_easing_luts.c     — pre-computed LUTs (2816 bytes total)
    sut_spring.c
    sut_color.c
    sut_animate.c
    sut_sequence.c
    sut_hal.c
    sut_ringbuf.c
    sut_signal.c
    sut_pool.c
  test/
    test_math.c
    test_easing.c
    test_spring.c
    test_color.c
    test_animate.c
    test_sequence.c
    test_ringbuf.c
    test_signal.c
    test_pool.c
  scripts/
    gen_easing_luts.py    — host tool to pre-compute LUTs as C arrays
```

---

### Task 1: Project scaffolding, types, allocator, errors, CMake

**Files:**
- Create: `lib/CMakeLists.txt`
- Create: `lib/include/sut_config.h`
- Create: `lib/include/sut_types.h`
- Create: `lib/include/sut_allocator.h`
- Create: `lib/include/sut_errors.h`
- Create: `lib/include/sut.h`
- Create: `lib/test/CMakeLists.txt`

- [ ] **Step 1: Create lib/CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.14)
project(smooth_ui_toolkit_c99 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Library sources
file(GLOB_RECURSE SUT_SRCS src/*.c)
include_directories(include)

add_library(sut STATIC ${SUT_SRCS})

# Test subdirectory
option(SUT_BUILD_TEST "Build tests" ON)
if(SUT_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
```

- [ ] **Step 2: Create lib/include/sut_config.h**

```c
#ifndef SUT_CONFIG_H
#define SUT_CONFIG_H

// Uncomment for platforms with hardware FPU (Cortex-M4F/M7, ESP32-S3, etc.)
// #define SUT_USE_FLOAT

#endif
```

- [ ] **Step 3: Create lib/include/sut_types.h**

```c
#ifndef SUT_TYPES_H
#define SUT_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include "sut_config.h"

#ifdef SUT_USE_FLOAT
    typedef float sut_real_t;
    #define SUT_FP_SCALE  1.0f
    #define SUT_FP_C(v)   (v)
#else
    typedef int32_t sut_real_t;
    #define SUT_FP_SCALE  65536
    #define SUT_FP_C(v)   ((sut_real_t)((v) * SUT_FP_SCALE))
#endif

#define SUT_REAL_FROM_INT(n)      ((sut_real_t)(n) * SUT_FP_SCALE)
#define SUT_REAL_FROM_FLOAT(f)    ((sut_real_t)((f) * SUT_FP_SCALE))
#define SUT_REAL_FROM_DOUBLE(d)   ((sut_real_t)((d) * SUT_FP_SCALE))
#define SUT_REAL_TO_FLOAT(r)      ((float)(r) / SUT_FP_SCALE)
#define SUT_REAL_TO_DOUBLE(r)     ((double)(r) / SUT_FP_SCALE)
#define SUT_REAL_TO_INT(r)        ((int)((r) / SUT_FP_SCALE))

#endif
```

- [ ] **Step 4: Create lib/include/sut_allocator.h**

```c
#ifndef SUT_ALLOCATOR_H
#define SUT_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    void* (*alloc)(size_t size, void* ctx);
    void  (*free)(void* ptr, void* ctx);
    void* ctx;
} sut_allocator_t;

void sut_allocator_set(sut_allocator_t* alloc);

void* sut_malloc(size_t size);
void  sut_free(void* ptr);

#endif
```

- [ ] **Step 5: Create lib/include/sut_errors.h**

```c
#ifndef SUT_ERRORS_H
#define SUT_ERRORS_H

#define SUT_OK          0
#define SUT_ERR_NOMEM  -1
#define SUT_ERR_FULL   -2
#define SUT_ERR_EMPTY  -3
#define SUT_ERR_INVAL  -4

#endif
```

- [ ] **Step 6: Create lib/include/sut.h (umbrella)**

```c
#ifndef SUT_H
#define SUT_H

#include "sut_types.h"
#include "sut_errors.h"
#include "sut_allocator.h"
#include "sut_math.h"
#include "sut_easing.h"
#include "sut_spring.h"
#include "sut_color.h"
#include "sut_animate.h"
#include "sut_sequence.h"
#include "sut_hal.h"
#include "sut_ringbuf.h"
#include "sut_signal.h"
#include "sut_pool.h"

#endif
```

- [ ] **Step 7: Create lib/test/CMakeLists.txt**

```cmake
function(add_sut_test name)
    add_executable(${name} ${name}.c)
    target_link_libraries(${name} sut)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_sut_test(test_math)
add_sut_test(test_easing)
add_sut_test(test_spring)
add_sut_test(test_color)
add_sut_test(test_animate)
add_sut_test(test_sequence)
add_sut_test(test_ringbuf)
add_sut_test(test_signal)
add_sut_test(test_pool)
```

- [ ] **Step 8: Verify the build compiles**

Run: `mkdir -p lib/build && cd lib/build && cmake .. && make`
Expected: library `libsut.a` is built (no .c files yet so it will be empty, but config headers work)

- [ ] **Step 9: Commit**

```bash
git add lib/
git commit -m "c99-port: add project scaffolding, types, allocator, CMake"
```

---

### Task 2: Math utilities

**Files:**
- Create: `lib/include/sut_math.h`
- Create: `lib/src/sut_math.c`
- Create: `lib/test/test_math.c`

- [ ] **Step 1: Create lib/include/sut_math.h**

```c
#ifndef SUT_MATH_H
#define SUT_MATH_H

#include "sut_types.h"

sut_real_t sut_clamp(sut_real_t v, sut_real_t min, sut_real_t max);
sut_real_t sut_lerp(sut_real_t a, sut_real_t b, sut_real_t t);
sut_real_t sut_map(sut_real_t v, sut_real_t a1, sut_real_t b1, sut_real_t a2, sut_real_t b2);

sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b);
sut_real_t sut_fp_div(sut_real_t a, sut_real_t b);
sut_real_t sut_fp_sqrt(sut_real_t v);

typedef struct { sut_real_t x, y; } sut_vec2_t;
sut_vec2_t sut_vec2(sut_real_t x, sut_real_t y);
sut_vec2_t sut_vec2_add(sut_vec2_t a, sut_vec2_t b);
sut_vec2_t sut_vec2_sub(sut_vec2_t a, sut_vec2_t b);
sut_vec2_t sut_vec2_mul(sut_vec2_t a, sut_real_t s);
sut_real_t sut_vec2_dot(sut_vec2_t a, sut_vec2_t b);
sut_real_t sut_vec2_length(sut_vec2_t a);
sut_vec2_t sut_vec2_normalize(sut_vec2_t a);
sut_vec2_t sut_vec2_lerp(sut_vec2_t a, sut_vec2_t b, sut_real_t t);

typedef struct { sut_real_t x, y, z, w; } sut_vec4_t;
sut_vec4_t sut_vec4(sut_real_t x, sut_real_t y, sut_real_t z, sut_real_t w);
sut_vec4_t sut_vec4_add(sut_vec4_t a, sut_vec4_t b);
sut_vec4_t sut_vec4_sub(sut_vec4_t a, sut_vec4_t b);
sut_vec4_t sut_vec4_mul(sut_vec4_t a, sut_real_t s);
sut_real_t sut_vec4_dot(sut_vec4_t a, sut_vec4_t b);
sut_vec4_t sut_vec4_lerp(sut_vec4_t a, sut_vec4_t b, sut_real_t t);

typedef struct { sut_real_t x, y, w, h; } sut_rect_t;
bool sut_rect_contains(sut_rect_t r, sut_vec2_t p);
bool sut_rect_overlaps(sut_rect_t a, sut_rect_t b);

#endif
```

- [ ] **Step 2: Create lib/src/sut_math.c (fixed-point mode)**

```c
#include "sut_math.h"
#include <math.h>

#ifdef SUT_USE_FLOAT

sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b) { return a * b; }
sut_real_t sut_fp_div(sut_real_t a, sut_real_t b) { return a / b; }
sut_real_t sut_fp_sqrt(sut_real_t v) { return sqrtf(v); }

#else

sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b) {
    return (sut_real_t)(((int64_t)a * b) >> 16);
}

sut_real_t sut_fp_div(sut_real_t a, sut_real_t b) {
    if (b == 0) return SUT_FP_C(0);
    int32_t neg = 1;
    uint64_t ua = (uint64_t)(a < 0 ? -a : a);
    uint64_t ub = (uint64_t)(b < 0 ? -b : b);
    if (a < 0) neg = -neg;
    if (b < 0) neg = -neg;
    uint32_t result = (uint32_t)((ua << 16) / ub);
    return (sut_real_t)(result * neg);
}

sut_real_t sut_fp_sqrt(sut_real_t v) {
    if (v <= 0) return 0;
    uint32_t val = (uint32_t)v;
    uint32_t root = (val + SUT_FP_C(1)) >> 1;
    for (int i = 0; i < 8; i++) {
        root = (root + (uint32_t)(((uint64_t)val << 16) / root)) >> 1;
    }
    return (sut_real_t)root;
}

#endif // SUT_USE_FLOAT

sut_real_t sut_clamp(sut_real_t v, sut_real_t min, sut_real_t max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

sut_real_t sut_lerp(sut_real_t a, sut_real_t b, sut_real_t t) {
    return a + sut_fp_mul(b - a, t);
}

sut_real_t sut_map(sut_real_t v, sut_real_t a1, sut_real_t b1, sut_real_t a2, sut_real_t b2) {
    sut_real_t t = sut_fp_div(v - a1, b1 - a1);
    return sut_lerp(a2, b2, t);
}

sut_vec2_t sut_vec2(sut_real_t x, sut_real_t y) {
    sut_vec2_t r = { x, y };
    return r;
}

sut_vec2_t sut_vec2_add(sut_vec2_t a, sut_vec2_t b) {
    sut_vec2_t r = { a.x + b.x, a.y + b.y };
    return r;
}

sut_vec2_t sut_vec2_sub(sut_vec2_t a, sut_vec2_t b) {
    sut_vec2_t r = { a.x - b.x, a.y - b.y };
    return r;
}

sut_vec2_t sut_vec2_mul(sut_vec2_t a, sut_real_t s) {
    sut_vec2_t r = { sut_fp_mul(a.x, s), sut_fp_mul(a.y, s) };
    return r;
}

sut_real_t sut_vec2_dot(sut_vec2_t a, sut_vec2_t b) {
    return sut_fp_mul(a.x, b.x) + sut_fp_mul(a.y, b.y);
}

sut_real_t sut_vec2_length(sut_vec2_t a) {
    return sut_fp_sqrt(sut_vec2_dot(a, a));
}

sut_vec2_t sut_vec2_normalize(sut_vec2_t a) {
    sut_real_t len = sut_vec2_length(a);
    if (len == 0) return a;
    sut_vec2_t r = { sut_fp_div(a.x, len), sut_fp_div(a.y, len) };
    return r;
}

sut_vec2_t sut_vec2_lerp(sut_vec2_t a, sut_vec2_t b, sut_real_t t) {
    sut_vec2_t r = {
        sut_lerp(a.x, b.x, t),
        sut_lerp(a.y, b.y, t)
    };
    return r;
}

sut_vec4_t sut_vec4(sut_real_t x, sut_real_t y, sut_real_t z, sut_real_t w) {
    sut_vec4_t r = { x, y, z, w };
    return r;
}

sut_vec4_t sut_vec4_add(sut_vec4_t a, sut_vec4_t b) {
    sut_vec4_t r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return r;
}

sut_vec4_t sut_vec4_sub(sut_vec4_t a, sut_vec4_t b) {
    sut_vec4_t r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return r;
}

sut_vec4_t sut_vec4_mul(sut_vec4_t a, sut_real_t s) {
    sut_vec4_t r = { sut_fp_mul(a.x, s), sut_fp_mul(a.y, s), sut_fp_mul(a.z, s), sut_fp_mul(a.w, s) };
    return r;
}

sut_real_t sut_vec4_dot(sut_vec4_t a, sut_vec4_t b) {
    return sut_fp_mul(a.x, b.x) + sut_fp_mul(a.y, b.y) + sut_fp_mul(a.z, b.z) + sut_fp_mul(a.w, b.w);
}

sut_vec4_t sut_vec4_lerp(sut_vec4_t a, sut_vec4_t b, sut_real_t t) {
    sut_vec4_t r = {
        sut_lerp(a.x, b.x, t),
        sut_lerp(a.y, b.y, t),
        sut_lerp(a.z, b.z, t),
        sut_lerp(a.w, b.w, t)
    };
    return r;
}

bool sut_rect_contains(sut_rect_t r, sut_vec2_t p) {
    return p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h;
}

bool sut_rect_overlaps(sut_rect_t a, sut_rect_t b) {
    if (a.x + a.w <= b.x || b.x + b.w <= a.x) return false;
    if (a.y + a.h <= b.y || b.y + b.h <= a.y) return false;
    return true;
}
```

- [ ] **Step 3: Write tests for basic math operations**

```c
// lib/test/test_math.c
#include <stdio.h>
#include <assert.h>
#include "sut_math.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; \
    printf("  TEST: %s ... ", name);

#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(cond) do { if (!(cond)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_clamp() {
    TEST("clamp");
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(5), SUT_FP_C(0), SUT_FP_C(10))) == 5);
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(-1), SUT_FP_C(0), SUT_FP_C(10))) == 0);
    CHECK(SUT_REAL_TO_INT(sut_clamp(SUT_FP_C(15), SUT_FP_C(0), SUT_FP_C(10))) == 10);
    PASS(); return 0;
}

static int test_lerp() {
    TEST("lerp 0→10 at t=0.5");
    sut_real_t r = sut_lerp(SUT_FP_C(0), SUT_FP_C(10), SUT_FP_C(0.5f));
    CHECK(SUT_REAL_TO_INT(r) == 5);
    PASS(); return 0;
}

static int test_fp_mul() {
    TEST("fp_mul 3 * 4");
    sut_real_t r = sut_fp_mul(SUT_FP_C(3), SUT_FP_C(4));
    CHECK(SUT_REAL_TO_INT(r) == 12);
    PASS(); return 0;
}

static int test_fp_div() {
    TEST("fp_div 10 / 2");
    sut_real_t r = sut_fp_div(SUT_FP_C(10), SUT_FP_C(2));
    CHECK(SUT_REAL_TO_INT(r) == 5);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_clamp();
    failed += test_lerp();
    failed += test_fp_mul();
    failed += test_fp_div();
    printf("Math: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify tests compile and pass**

Run: `cd lib/build && make test_math && ./test/test_math`
Expected: All tests PASS

- [ ] **Step 5: Commit**

```bash
git add lib/include/sut_math.h lib/src/sut_math.c lib/test/test_math.c
git commit -m "c99-port: add fixed-point math utilities + tests"
```

---

### Task 3: Easing functions

**Files:**
- Create: `lib/include/sut_easing.h`
- Create: `lib/src/sut_easing.c`
- Create: `lib/scripts/gen_easing_luts.py`
- Create: `lib/src/sut_easing_luts.c`
- Create: `lib/test/test_easing.c`

- [ ] **Step 1: Create lib/include/sut_easing.h**

```c
#ifndef SUT_EASING_H
#define SUT_EASING_H

#include "sut_types.h"

typedef sut_real_t (*sut_easing_fn_t)(sut_real_t t);

sut_real_t sut_ease_linear(sut_real_t t);
sut_real_t sut_ease_quad_in(sut_real_t t);
sut_real_t sut_ease_quad_out(sut_real_t t);
sut_real_t sut_ease_quad_in_out(sut_real_t t);
sut_real_t sut_ease_cubic_in(sut_real_t t);
sut_real_t sut_ease_cubic_out(sut_real_t t);
sut_real_t sut_ease_cubic_in_out(sut_real_t t);
sut_real_t sut_ease_quart_in(sut_real_t t);
sut_real_t sut_ease_quart_out(sut_real_t t);
sut_real_t sut_ease_quart_in_out(sut_real_t t);
sut_real_t sut_ease_quint_in(sut_real_t t);
sut_real_t sut_ease_quint_out(sut_real_t t);
sut_real_t sut_ease_quint_in_out(sut_real_t t);

extern const sut_real_t sut_ease_sine_in_lut[257];
extern const sut_real_t sut_ease_sine_out_lut[257];
extern const sut_real_t sut_ease_sine_in_out_lut[257];
extern const sut_real_t sut_ease_expo_in_lut[257];
extern const sut_real_t sut_ease_expo_out_lut[257];
extern const sut_real_t sut_ease_expo_in_out_lut[257];
extern const sut_real_t sut_ease_circ_in_lut[257];
extern const sut_real_t sut_ease_circ_out_lut[257];
extern const sut_real_t sut_ease_circ_in_out_lut[257];
extern const sut_real_t sut_ease_back_in_lut[257];
extern const sut_real_t sut_ease_back_out_lut[257];
extern const sut_real_t sut_ease_back_in_out_lut[257];
extern const sut_real_t sut_ease_elastic_in_lut[257];
extern const sut_real_t sut_ease_elastic_out_lut[257];
extern const sut_real_t sut_ease_elastic_in_out_lut[257];
extern const sut_real_t sut_ease_bounce_in_lut[257];
extern const sut_real_t sut_ease_bounce_out_lut[257];
extern const sut_real_t sut_ease_bounce_in_out_lut[257];

sut_real_t sut_ease_lut(const sut_real_t* lut, sut_real_t t);

sut_real_t sut_ease_cubic_bezier(sut_real_t t, sut_real_t x1, sut_real_t y1, sut_real_t x2, sut_real_t y2);

#endif
```

- [ ] **Step 2: Create lib/src/sut_easing.c — polynomial easings**

```c
#include "sut_easing.h"

#ifndef SUT_USE_FLOAT

static sut_real_t fp_pow2(sut_real_t t) {
    return sut_fp_mul(t, t);
}
static sut_real_t fp_pow3(sut_real_t t) {
    return sut_fp_mul(sut_fp_mul(t, t), t);
}
static sut_real_t fp_pow4(sut_real_t t) {
    return sut_fp_mul(sut_fp_mul(sut_fp_mul(t, t), t), t);
}
static sut_real_t fp_pow5(sut_real_t t) {
    sut_real_t t2 = sut_fp_mul(t, t);
    return sut_fp_mul(sut_fp_mul(t2, t2), t);
}

#endif

sut_real_t sut_ease_linear(sut_real_t t) {
    return t;
}

#ifdef SUT_USE_FLOAT

sut_real_t sut_ease_quad_in(sut_real_t t) { return t * t; }
sut_real_t sut_ease_quad_out(sut_real_t t) { return t * (2.0f - t); }
sut_real_t sut_ease_quad_in_out(sut_real_t t) {
    if (t < 0.5f) return 2.0f * t * t;
    return -1.0f + (4.0f - 2.0f * t) * t;
}
sut_real_t sut_ease_cubic_in(sut_real_t t) { return t * t * t; }
sut_real_t sut_ease_cubic_out(sut_real_t t) {
    sut_real_t f = t - 1.0f;
    return f * f * f + 1.0f;
}
sut_real_t sut_ease_cubic_in_out(sut_real_t t) {
    if (t < 0.5f) return 4.0f * t * t * t;
    sut_real_t f = t - 1.0f;
    return 1.0f + 4.0f * f * f * f;
}
sut_real_t sut_ease_quart_in(sut_real_t t) { return t * t * t * t; }
sut_real_t sut_ease_quart_out(sut_real_t t) {
    sut_real_t f = t - 1.0f;
    return 1.0f - f * f * f * f;
}
sut_real_t sut_ease_quart_in_out(sut_real_t t) {
    if (t < 0.5f) return 8.0f * t * t * t * t;
    sut_real_t f = t - 1.0f;
    return 1.0f - 8.0f * f * f * f * f;
}
sut_real_t sut_ease_quint_in(sut_real_t t) {
    return t * t * t * t * t;
}
sut_real_t sut_ease_quint_out(sut_real_t t) {
    sut_real_t f = t - 1.0f;
    return f * f * f * f * f + 1.0f;
}
sut_real_t sut_ease_quint_in_out(sut_real_t t) {
    if (t < 0.5f) return 16.0f * t * t * t * t * t;
    sut_real_t f = t - 1.0f;
    return 1.0f + 16.0f * f * f * f * f * f;
}

#else

sut_real_t sut_ease_quad_in(sut_real_t t) { return fp_pow2(t); }
sut_real_t sut_ease_quad_out(sut_real_t t) {
    return t * (SUT_FP_C(2) - t);
}
sut_real_t sut_ease_quad_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(SUT_FP_C(2), fp_pow2(t));
    sut_real_t f = t - SUT_FP_C(1);
    return SUT_FP_C(1) - sut_fp_mul(SUT_FP_C(2), fp_pow2(f));
}
sut_real_t sut_ease_cubic_in(sut_real_t t) { return fp_pow3(t); }
sut_real_t sut_ease_cubic_out(sut_real_t t) {
    sut_real_t f = t - SUT_FP_C(1);
    return fp_pow3(f) + SUT_FP_C(1);
}
sut_real_t sut_ease_cubic_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(SUT_FP_C(4), fp_pow3(t));
    sut_real_t f = t - SUT_FP_C(1);
    return SUT_FP_C(1) + sut_fp_mul(SUT_FP_C(4), fp_pow3(f));
}
sut_real_t sut_ease_quart_in(sut_real_t t) { return fp_pow4(t); }
sut_real_t sut_ease_quart_out(sut_real_t t) {
    sut_real_t f = t - SUT_FP_C(1);
    return SUT_FP_C(1) - fp_pow4(f);
}
sut_real_t sut_ease_quart_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(SUT_FP_C(8), fp_pow4(t));
    sut_real_t f = t - SUT_FP_C(1);
    return SUT_FP_C(1) - sut_fp_mul(SUT_FP_C(8), fp_pow4(f));
}
sut_real_t sut_ease_quint_in(sut_real_t t) { return fp_pow5(t); }
sut_real_t sut_ease_quint_out(sut_real_t t) {
    sut_real_t f = t - SUT_FP_C(1);
    return fp_pow5(f) + SUT_FP_C(1);
}
sut_real_t sut_ease_quint_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(SUT_FP_C(16), fp_pow5(t));
    sut_real_t f = t - SUT_FP_C(1);
    return SUT_FP_C(1) + sut_fp_mul(SUT_FP_C(16), fp_pow5(f));
}

#endif

sut_real_t sut_ease_lut(const sut_real_t* lut, sut_real_t t) {
    uint32_t u = (uint32_t)t;
    uint16_t idx = u >> 8;
    if (idx > 255) return SUT_FP_C(1);
    sut_real_t f0 = lut[idx];
    sut_real_t f1 = lut[idx + 1];
    sut_real_t frac = (sut_real_t)(u & 0xFF) << 8;
    return f0 + sut_fp_mul(f1 - f0, frac);
}

// LUT-based wrappers (both float and fixed-point mode use LUTs)
sut_real_t sut_ease_sine_in(sut_real_t t) { return sut_ease_lut(sut_ease_sine_in_lut, t); }
sut_real_t sut_ease_sine_out(sut_real_t t) { return sut_ease_lut(sut_ease_sine_out_lut, t); }
sut_real_t sut_ease_sine_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_sine_in_out_lut, t); }
sut_real_t sut_ease_expo_in(sut_real_t t) { return sut_ease_lut(sut_ease_expo_in_lut, t); }
sut_real_t sut_ease_expo_out(sut_real_t t) { return sut_ease_lut(sut_ease_expo_out_lut, t); }
sut_real_t sut_ease_expo_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_expo_in_out_lut, t); }
sut_real_t sut_ease_circ_in(sut_real_t t) { return sut_ease_lut(sut_ease_circ_in_lut, t); }
sut_real_t sut_ease_circ_out(sut_real_t t) { return sut_ease_lut(sut_ease_circ_out_lut, t); }
sut_real_t sut_ease_circ_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_circ_in_out_lut, t); }
sut_real_t sut_ease_back_in(sut_real_t t) { return sut_ease_lut(sut_ease_back_in_lut, t); }
sut_real_t sut_ease_back_out(sut_real_t t) { return sut_ease_lut(sut_ease_back_out_lut, t); }
sut_real_t sut_ease_back_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_back_in_out_lut, t); }
sut_real_t sut_ease_elastic_in(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_in_lut, t); }
sut_real_t sut_ease_elastic_out(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_out_lut, t); }
sut_real_t sut_ease_elastic_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_in_out_lut, t); }
sut_real_t sut_ease_bounce_in(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_in_lut, t); }
sut_real_t sut_ease_bounce_out(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_out_lut, t); }
sut_real_t sut_ease_bounce_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_in_out_lut, t); }

sut_real_t sut_ease_cubic_bezier(sut_real_t t, sut_real_t x1, sut_real_t y1, sut_real_t x2, sut_real_t y2) {
    // De Casteljau in fixed-point
    sut_real_t u = SUT_FP_C(1) - t;
    // P0=(0,0), P1=(x1,y1), P2=(x2,y2), P3=(1,1)
    // First level
    sut_real_t ax = u + sut_fp_mul(t, x1);
    sut_real_t bx = sut_fp_mul(u, x1) + sut_fp_mul(t, x2);
    sut_real_t cx = sut_fp_mul(u, x2) + t; // t * 1
    // Second level
    sut_real_t dx = sut_fp_mul(u, ax) + sut_fp_mul(t, bx);
    sut_real_t ex = sut_fp_mul(u, bx) + sut_fp_mul(t, cx);
    // Third level — x coordinate at t
    sut_real_t x = sut_fp_mul(u, dx) + sut_fp_mul(t, ex);
    // Same for Y
    sut_real_t ay = sut_fp_mul(u, y1);
    sut_real_t by = sut_fp_mul(u, y1) + sut_fp_mul(t, y2);
    sut_real_t cy = sut_fp_mul(u, y2) + SUT_FP_C(1);
    sut_real_t dy = sut_fp_mul(u, ay) + sut_fp_mul(t, by);
    sut_real_t ey = sut_fp_mul(u, by) + sut_fp_mul(t, cy);
    sut_real_t y = sut_fp_mul(u, dy) + sut_fp_mul(t, ey);
    return y;
}
```

- [ ] **Step 3: Create lib/scripts/gen_easing_luts.py — LUT generator**

```python
#!/usr/bin/env python3
"""Generate sut_easing_luts.c with pre-computed 257-entry Q16.16 LUTs."""
import math

SCALE = 65536
LUT_SIZE = 257  # 256 intervals + last point

def to_q16(v):
    """Float in [0,1] → Q16.16 fixed-point int32_t"""
    return int(round(max(0.0, min(1.0, v)) * SCALE))

def ease_sine_in(t): return 1 - math.cos(t * math.pi / 2)
def ease_sine_out(t): return math.sin(t * math.pi / 2)
def ease_sine_in_out(t): return -(math.cos(math.pi * t) - 1) / 2

def ease_expo_in(t): return 0 if t == 0 else 2 ** (10 * t - 10)
def ease_expo_out(t): return 1 if t == 1 else 1 - 2 ** (-10 * t)
def ease_expo_in_out(t):
    if t == 0: return 0
    if t == 1: return 1
    if t < 0.5: return 2 ** (20 * t - 10) / 2
    return (2 - 2 ** (-20 * t + 10)) / 2

def ease_circ_in(t): return 1 - math.sqrt(1 - t ** 2)
def ease_circ_out(t): return math.sqrt(1 - (t - 1) ** 2)
def ease_circ_in_out(t):
    if t < 0.5: return (1 - math.sqrt(1 - (2 * t) ** 2)) / 2
    return (math.sqrt(1 - (-2 * t + 2) ** 2) + 1) / 2

c1, c2, c3 = 1.70158, 2.59491, 2.70158
def ease_back_in(t): return c3 * t ** 3 - c1 * t ** 2
def ease_back_out(t): return 1 + c3 * (t - 1) ** 3 + c1 * (t - 1) ** 2
def ease_back_in_out(t):
    if t < 0.5: return ((2 * t) ** 2 * ((c2 + 1) * 2 * t - c2)) / 2
    return ((2 * t - 2) ** 2 * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2

def ease_elastic_in(t):
    if t == 0 or t == 1: return t
    return -(2 ** (10 * t - 10)) * math.sin((t * 10 - 10.75) * 2 * math.pi / 3)
def ease_elastic_out(t):
    if t == 0 or t == 1: return t
    return 2 ** (-10 * t) * math.sin((t * 10 - 0.75) * 2 * math.pi / 3) + 1
def ease_elastic_in_out(t):
    if t == 0 or t == 1: return t
    if t < 0.5:
        return -(2 ** (20 * t - 10) * math.sin((20 * t - 11.125) * 2 * math.pi / 4.5)) / 2
    return (2 ** (-20 * t + 10) * math.sin((20 * t - 11.125) * 2 * math.pi / 4.5)) / 2 + 1

n1, d1 = 7.5625, 2.75
def ease_bounce_out(t):
    if t < 1 / d1: return n1 * t ** 2
    elif t < 2 / d1: return n1 * (t - 1.5 / d1) ** 2 + 0.75
    elif t < 2.5 / d1: return n1 * (t - 2.25 / d1) ** 2 + 0.9375
    else: return n1 * (t - 2.625 / d1) ** 2 + 0.984375
def ease_bounce_in(t): return 1 - ease_bounce_out(1 - t)
def ease_bounce_in_out(t):
    if t < 0.5: return (1 - ease_bounce_out(1 - 2 * t)) / 2
    return (1 + ease_bounce_out(2 * t - 1)) / 2

EASINGS = [
    ("sine_in", ease_sine_in), ("sine_out", ease_sine_out), ("sine_in_out", ease_sine_in_out),
    ("expo_in", ease_expo_in), ("expo_out", ease_expo_out), ("expo_in_out", ease_expo_in_out),
    ("circ_in", ease_circ_in), ("circ_out", ease_circ_out), ("circ_in_out", ease_circ_in_out),
    ("back_in", ease_back_in), ("back_out", ease_back_out), ("back_in_out", ease_back_in_out),
    ("elastic_in", ease_elastic_in), ("elastic_out", ease_elastic_out), ("elastic_in_out", ease_elastic_in_out),
    ("bounce_in", ease_bounce_in), ("bounce_out", ease_bounce_out), ("bounce_in_out", ease_bounce_in_out),
]

HEADER = """/* Auto-generated by gen_easing_luts.py — do not edit */
#include "sut_easing.h"

"""

print(HEADER, end="")
for name, func in EASINGS:
    values = [to_q16(func(i / (LUT_SIZE - 1))) for i in range(LUT_SIZE)]
    print(f"const sut_real_t sut_ease_{name}_lut[{LUT_SIZE}] = {{")
    for i in range(0, LUT_SIZE, 8):
        chunk = values[i:i+8]
        print("    " + ", ".join(f"{v:6d}" for v in chunk) + ",")
    print("};\n")
```

- [ ] **Step 4: Generate LUT source file**

Run: `python3 lib/scripts/gen_easing_luts.py > lib/src/sut_easing_luts.c`
Expected: 18 LUT arrays, each 257 entries, output written to .c file

Verify: `wc -l lib/src/sut_easing_luts.c` should show ~500 lines

- [ ] **Step 5: Write easing test**

```c
// lib/test/test_easing.c
#include <stdio.h>
#include <assert.h>
#include "sut_easing.h"

static int tests_run = 0;
static int tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_linear_bounds() {
    TEST("linear bounds");
    CHECK(sut_ease_linear(SUT_FP_C(0)) == SUT_FP_C(0));
    CHECK(sut_ease_linear(SUT_FP_C(1)) == SUT_FP_C(1));
    CHECK(sut_ease_linear(SUT_FP_C(0.5f)) == SUT_FP_C(0.5f));
    PASS(); return 0;
}

static int test_quad_out_bounds() {
    TEST("quad_out bounds");
    CHECK(sut_ease_quad_out(SUT_FP_C(0)) == SUT_FP_C(0));
    CHECK(sut_ease_quad_out(SUT_FP_C(1)) == SUT_FP_C(1));
    PASS(); return 0;
}

static int test_lut_bounds() {
    TEST("lut bounds");
    CHECK(sut_ease_sine_in(SUT_FP_C(0)) == SUT_FP_C(0));
    CHECK(sut_ease_sine_out(SUT_FP_C(1)) == SUT_FP_C(1));
    CHECK(sut_ease_bounce_out(SUT_FP_C(0)) == SUT_FP_C(0));
    CHECK(sut_ease_bounce_out(SUT_FP_C(1)) == SUT_FP_C(1));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_linear_bounds();
    failed += test_quad_out_bounds();
    failed += test_lut_bounds();
    printf("Easing: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 6: Verify tests compile and pass**

Run: `cd lib/build && cmake .. && make test_easing && ./test/test_easing`
Expected: All tests PASS

- [ ] **Step 7: Commit**

```bash
git add lib/include/sut_easing.h lib/src/sut_easing.c lib/src/sut_easing_luts.c lib/scripts/gen_easing_luts.py lib/test/test_easing.c
git commit -m "c99-port: add fixed-point easing functions + LUT generation"
```

---

### Task 4: Spring physics

**Files:**
- Create: `lib/include/sut_spring.h`
- Create: `lib/src/sut_spring.c`
- Create: `lib/test/test_spring.c`

- [ ] **Step 1: Create lib/include/sut_spring.h**

```c
#ifndef SUT_SPRING_H
#define SUT_SPRING_H

#include "sut_types.h"

typedef struct {
    sut_real_t stiffness;
    sut_real_t damping;
    sut_real_t mass;
} sut_spring_params_t;

typedef struct {
    sut_real_t velocity;
} sut_spring_state_t;

// force = -k*(x - target) - c*v;  a = force/m;  v += a*dt;  x += v*dt
void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt);

#endif
```

- [ ] **Step 2: Create lib/src/sut_spring.c**

```c
#include "sut_spring.h"

void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt) {
    sut_real_t displacement = *x - target;
    // force = -k*displacement - c*velocity
    sut_real_t force = sut_fp_mul(p->stiffness, displacement);
    force = -(force + sut_fp_mul(p->damping, s->velocity));
    // acceleration = force / mass
    sut_real_t accel = sut_fp_div(force, p->mass);
    // Euler integration
    s->velocity += sut_fp_mul(accel, dt);
    *x += sut_fp_mul(s->velocity, dt);
}
```

- [ ] **Step 3: Create spring test**

```c
// lib/test/test_spring.c
#include <stdio.h>
#include <math.h>
#include "sut_spring.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_spring_converges() {
    TEST("spring converges to target");
    sut_real_t x = SUT_FP_C(0);
    sut_spring_state_t s = { 0 };
    sut_spring_params_t p = { SUT_FP_C(100), SUT_FP_C(10), SUT_FP_C(1) };
    sut_real_t target = SUT_FP_C(10);
    // Run 1000 steps at dt = 1/60
    sut_real_t dt = SUT_FP_C(1.0f / 60.0f);
    for (int i = 0; i < 1000; i++) {
        sut_spring_step(&x, &s, &p, target, dt);
    }
    // Should be within 1% of target
    sut_real_t error = x - target;
    if (error < 0) error = -error;
    CHECK(error < SUT_FP_C(0.01f));
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_spring_converges();
    printf("Spring: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify tests pass**

Run: `cd lib/build && make test_spring && ./test/test_spring`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add lib/include/sut_spring.h lib/src/sut_spring.c lib/test/test_spring.c
git commit -m "c99-port: add Euler-integration spring physics + test"
```

---

### Task 5: Animation engine

**Files:**
- Create: `lib/include/sut_animate.h`
- Create: `lib/src/sut_animate.c`
- Create: `lib/test/test_animate.c`

- [ ] **Step 1: Create lib/include/sut_animate.h**

```c
#ifndef SUT_ANIMATE_H
#define SUT_ANIMATE_H

#include "sut_types.h"
#include "sut_easing.h"
#include "sut_spring.h"

typedef enum {
    SUT_ANIMATE_EASING,
    SUT_ANIMATE_SPRING
} sut_animate_mode_t;

typedef struct {
    sut_real_t from;
    sut_real_t to;
    sut_real_t current;
    sut_real_t elapsed;
    sut_real_t delay;

    sut_real_t velocity;           // spring state

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void    (*on_update)(void* ctx, sut_real_t value);
    void    (*on_complete)(void* ctx);
    void*   ctx;

    uint8_t mode;
    uint8_t repeat;
    uint8_t repeat_count;
} sut_animate_t;

void sut_animate_init_easing(sut_animate_t* a, sut_real_t from, sut_real_t to, sut_real_t duration);
void sut_animate_init_spring(sut_animate_t* a, sut_real_t from, sut_real_t to);
bool sut_animate_update(sut_animate_t* a, sut_real_t dt);
void sut_animate_reset(sut_animate_t* a);

// --- Vec2 animation ---
typedef struct {
    sut_vec2_t from;
    sut_vec2_t to;
    sut_vec2_t current;
    sut_real_t elapsed;
    sut_real_t delay;

    sut_vec2_t velocity;

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, sut_vec2_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} sut_vec2_animate_t;

void sut_vec2_animate_init_easing(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to, sut_real_t duration);
void sut_vec2_animate_init_spring(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to);
void sut_vec2_animate_move_to(sut_vec2_animate_t* a, sut_vec2_t target);
bool sut_vec2_animate_update(sut_vec2_animate_t* a, sut_real_t dt);

// --- Vec4 animation ---
typedef struct {
    sut_vec4_t from;
    sut_vec4_t to;
    sut_vec4_t current;
    sut_real_t elapsed;
    sut_real_t delay;

    sut_vec4_t velocity;

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, sut_vec4_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} sut_vec4_animate_t;

void sut_vec4_animate_init_easing(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to, sut_real_t duration);
void sut_vec4_animate_init_spring(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to);
void sut_vec4_animate_move_to(sut_vec4_animate_t* a, sut_vec4_t target);
bool sut_vec4_animate_update(sut_vec4_animate_t* a, sut_real_t dt);

#endif
```

- [ ] **Step 2: Create lib/src/sut_animate.c**

```c
#include "sut_animate.h"
#include <string.h>

// --- Value animation ---

void sut_animate_init_easing(sut_animate_t* a, sut_real_t from, sut_real_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_animate_init_spring(sut_animate_t* a, sut_real_t from, sut_real_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

bool sut_animate_update(sut_animate_t* a, sut_real_t dt) {
    // Handle delay
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;  // carry-over remaining dt
        a->delay = 0;
    }

    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_state_t ss = { a->velocity };
        sut_spring_step(&a->current, &ss, &a->config.spring, a->to, dt);
        a->velocity = ss.velocity;
        // Spring never "finishes" — user checks done() via velocity threshold
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    // Easing mode
    a->elapsed += dt;
    sut_real_t t = a->elapsed;
    if (a->config.easing.duration > 0)
        t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->repeat > 0 && a->repeat_count < a->repeat) {
            a->repeat_count++;
            a->elapsed = 0;
            a->current = a->from;
            return false;
        }
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

void sut_animate_reset(sut_animate_t* a) {
    a->current = a->from;
    a->elapsed = 0;
    a->velocity = 0;
    a->repeat_count = 0;
}
```

// --- Vec2 animation ---

void sut_vec2_animate_init_easing(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_vec2_animate_init_spring(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

void sut_vec2_animate_move_to(sut_vec2_animate_t* a, sut_vec2_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity = sut_vec2(0, 0);
}

bool sut_vec2_animate_update(sut_vec2_animate_t* a, sut_real_t dt) {
    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_step(&a->current.x, (sut_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        sut_spring_step(&a->current.y, (sut_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }
    a->elapsed += dt;
    sut_real_t t = a->elapsed;
    if (a->config.easing.duration > 0)
        t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_vec2_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

// --- Vec4 animation (same pattern) ---

void sut_vec4_animate_init_easing(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_vec4_animate_init_spring(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

void sut_vec4_animate_move_to(sut_vec4_animate_t* a, sut_vec4_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity = sut_vec4(0, 0, 0, 0);
}

bool sut_vec4_animate_update(sut_vec4_animate_t* a, sut_real_t dt) {
    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_step(&a->current.x, (sut_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        sut_spring_step(&a->current.y, (sut_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        sut_spring_step(&a->current.z, (sut_spring_state_t*)&a->velocity.z, &a->config.spring, a->to.z, dt);
        sut_spring_step(&a->current.w, (sut_spring_state_t*)&a->velocity.w, &a->config.spring, a->to.w, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }
    a->elapsed += dt;
    sut_real_t t = a->elapsed;
    if (a->config.easing.duration > 0)
        t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_vec4_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

- [ ] **Step 3: Create animate test**

```c
// lib/test/test_animate.c
#include <stdio.h>
#include "sut_animate.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_easing_anim_completes() {
    TEST("easing anim from 0 to 10 over 1s");
    sut_animate_t a;
    sut_animate_init_easing(&a, SUT_FP_C(0), SUT_FP_C(10), SUT_FP_C(1));
    bool done = sut_animate_update(&a, SUT_FP_C(1));
    CHECK(done == true);
    CHECK(SUT_REAL_TO_INT(a.current) == 10);
    PASS(); return 0;
}

static int test_easing_anim_partial() {
    TEST("easing anim at t=0.5 (linear → mid)");
    sut_animate_t a;
    sut_animate_init_easing(&a, SUT_FP_C(0), SUT_FP_C(10), SUT_FP_C(1));
    a.config.easing.easing = sut_ease_linear;
    bool done = sut_animate_update(&a, SUT_FP_C(0.5f));
    CHECK(done == false);
    CHECK(SUT_REAL_TO_INT(a.current) == 5);
    PASS(); return 0;
}

static int test_spring_anim() {
    TEST("spring anim from 0 to 10");
    sut_animate_t a;
    sut_animate_init_spring(&a, SUT_FP_C(0), SUT_FP_C(10));
    sut_animate_update(&a, SUT_FP_C(1.0f / 60.0f));
    CHECK(a.current > 0);  // should have moved toward target
    CHECK(a.current < SUT_FP_C(10));  // not overshot wildly
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_easing_anim_completes();
    failed += test_easing_anim_partial();
    failed += test_spring_anim();
    printf("Animate: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify tests pass**

Run: `cd lib/build && make test_animate && ./test/test_animate`
Expected: All PASS

- [ ] **Step 5: Commit**

```bash
git add lib/include/sut_animate.h lib/src/sut_animate.c lib/test/test_animate.c
git commit -m "c99-port: add animation engine (easing + spring mode) + tests"
```

---

### Task 6: Animation sequence

- [ ] **Step 1: Create lib/include/sut_sequence.h**

```c
#ifndef SUT_SEQUENCE_H
#define SUT_SEQUENCE_H

#include "sut_types.h"
#include "sut_easing.h"

typedef struct {
    sut_real_t      target;
    sut_real_t      duration;
    sut_easing_fn_t easing;
} sut_animate_step_t;

typedef struct {
    sut_animate_step_t* steps;
    void (*on_step)(void* ctx, int index);
    void (*on_complete)(void* ctx);
    void*   ctx;
    uint16_t capacity;
    uint16_t count;
    uint16_t current;
    sut_real_t elapsed;
    sut_real_t from;
} sut_sequence_t;

int   sut_sequence_init(sut_sequence_t* s, uint16_t capacity);
void  sut_sequence_deinit(sut_sequence_t* s);
int   sut_sequence_push(sut_sequence_t* s, sut_animate_step_t step);
void  sut_sequence_clear(sut_sequence_t* s);
bool  sut_sequence_update(sut_sequence_t* s, sut_real_t dt);
void  sut_sequence_play(sut_sequence_t* s);

#endif
```

- [ ] **Step 2: Create lib/src/sut_sequence.c**

```c
#include "sut_sequence.h"
#include <string.h>

int sut_sequence_init(sut_sequence_t* s, uint16_t capacity) {
    memset(s, 0, sizeof(*s));
    s->steps = (sut_animate_step_t*)sut_malloc(capacity * sizeof(sut_animate_step_t));
    if (!s->steps) return SUT_ERR_NOMEM;
    s->capacity = capacity;
    s->current = 0;
    s->count = 0;
    s->elapsed = 0;
    return SUT_OK;
}

void sut_sequence_deinit(sut_sequence_t* s) {
    sut_free(s->steps);
    memset(s, 0, sizeof(*s));
}

int sut_sequence_push(sut_sequence_t* s, sut_animate_step_t step) {
    if (s->count >= s->capacity) return SUT_ERR_FULL;
    s->steps[s->count++] = step;
    return SUT_OK;
}

void sut_sequence_clear(sut_sequence_t* s) {
    s->count = 0;
    s->current = 0;
    s->elapsed = 0;
}

void sut_sequence_play(sut_sequence_t* s) {
    s->current = 0;
    s->elapsed = 0;
    if (s->count > 0 && s->on_step)
        s->on_step(s->ctx, 0);
}

bool sut_sequence_update(sut_sequence_t* s, sut_real_t dt) {
    if (s->current >= s->count) return true;

    sut_animate_step_t* step = &s->steps[s->current];
    s->elapsed += dt;

    sut_real_t t = s->elapsed;
    if (step->duration > 0)
        t = sut_fp_div(s->elapsed, step->duration);

    if (t >= SUT_FP_C(1)) {
        s->current++;
        s->elapsed = 0;
        if (s->current >= s->count) {
            if (s->on_complete) s->on_complete(s->ctx);
            return true;
        }
        if (s->on_step) s->on_step(s->ctx, s->current);
        return false;
    }

    return false;
}
```

- [ ] **Step 3: Create sequence test**

```c
// lib/test/test_sequence.c
#include <stdio.h>
#include "sut_sequence.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int step_index = 0;

static void on_step(void* ctx, int index) { step_index = index; }
static int complete_called = 0;
static void on_complete(void* ctx) { complete_called = 1; }

static int test_seq_3_steps() {
    TEST("3 steps, each 0.5s, linear");
    sut_sequence_t seq;
    sut_sequence_init(&seq, 10);
    seq.on_step = on_step;
    seq.on_complete = on_complete;
    sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(10), SUT_FP_C(0.5f), sut_ease_linear });
    sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(20), SUT_FP_C(0.5f), sut_ease_linear });
    sut_sequence_push(&seq, (sut_animate_step_t){ SUT_FP_C(30), SUT_FP_C(0.5f), sut_ease_linear });
    sut_sequence_play(&seq);

    // Update 1.6s total — should complete all 3 steps
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.5f)) == false);
    CHECK(step_index == 0);
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.5f)) == false);
    CHECK(step_index == 1);
    CHECK(sut_sequence_update(&seq, SUT_FP_C(0.6f)) == true);  // done
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
```

- [ ] **Step 4: Verify + commit**

Run: `cd lib/build && make test_sequence && ./test/test_sequence`
Expected: PASS

```bash
git add lib/include/sut_sequence.h lib/src/sut_sequence.c lib/test/test_sequence.c
git commit -m "c99-port: add animation sequence + test"
```

---

### Task 7: Color utilities

- [ ] **Step 1: Create lib/include/sut_color.h**

```c
#ifndef SUT_COLOR_H
#define SUT_COLOR_H

#include <stdint.h>
#include "sut_types.h"

typedef struct { uint8_t r, g, b; } sut_rgb_t;
typedef struct { uint8_t r, g, b, a; } sut_rgba_t;

sut_rgb_t  sut_rgb_from_hex(uint32_t hex);
uint32_t   sut_rgb_to_hex(sut_rgb_t c);
sut_rgb_t  sut_rgb_from_hsv(sut_real_t h, sut_real_t s, sut_real_t v);
sut_rgb_t  sut_rgb_blend_opacity(sut_rgb_t bg, sut_rgb_t fg, sut_real_t opacity);
sut_rgb_t  sut_rgb_blend_difference(sut_rgb_t bg, sut_rgb_t fg);

#endif
```

- [ ] **Step 2: Create lib/src/sut_color.c**

```c
#include "sut_color.h"

sut_rgb_t sut_rgb_from_hex(uint32_t hex) {
    sut_rgb_t c;
    c.r = (hex >> 16) & 0xFF;
    c.g = (hex >> 8) & 0xFF;
    c.b = hex & 0xFF;
    return c;
}

uint32_t sut_rgb_to_hex(sut_rgb_t c) {
    return ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | c.b;
}

sut_rgb_t sut_rgb_from_hsv(sut_real_t h, sut_real_t s, sut_real_t v) {
    // Standard HSV → RGB algorithm, h in [0, SUT_FP_C(360)]
    sut_rgb_t c = {0, 0, 0};
    #ifdef SUT_USE_FLOAT
        // float implementation
        float hf = SUT_REAL_TO_FLOAT(h) / 60.0f;
        float sf = SUT_REAL_TO_FLOAT(s);
        float vf = SUT_REAL_TO_FLOAT(v);
        int hi = (int)hf % 6;
        float f = hf - (int)hf;
        float p = vf * (1.0f - sf);
        float q = vf * (1.0f - sf * f);
        float t = vf * (1.0f - sf * (1.0f - f));
        float rgb[3] = {0};
        switch (hi) {
            case 0: rgb[0]=vf; rgb[1]=t;  rgb[2]=p;  break;
            case 1: rgb[0]=q;  rgb[1]=vf; rgb[2]=p;  break;
            case 2: rgb[0]=p;  rgb[1]=vf; rgb[2]=t;  break;
            case 3: rgb[0]=p;  rgb[1]=q;  rgb[2]=vf; break;
            case 4: rgb[0]=t;  rgb[1]=p;  rgb[2]=vf; break;
            case 5: rgb[0]=vf; rgb[1]=p;  rgb[2]=q;  break;
        }
        c.r = (uint8_t)(rgb[0] * 255.0f);
        c.g = (uint8_t)(rgb[1] * 255.0f);
        c.b = (uint8_t)(rgb[2] * 255.0f);
    #else
        // Q16.16 fixed-point implementation
        sut_real_t hf = sut_fp_div(h, SUT_FP_C(60));
        int hi = (SUT_REAL_TO_INT(hf)) % 6;
        if (hi < 0) hi += 6;
        sut_real_t f = hf - SUT_REAL_TO_INT(hf) * SUT_FP_SCALE;
        sut_real_t p = sut_fp_mul(v, SUT_FP_C(1) - s);
        sut_real_t q = sut_fp_mul(v, SUT_FP_C(1) - sut_fp_mul(s, f));
        sut_real_t t_val = sut_fp_mul(v, SUT_FP_C(1) - sut_fp_mul(s, SUT_FP_C(1) - f));
        sut_real_t rgb[3] = {0};
        switch (hi) {
            case 0: rgb[0]=v; rgb[1]=t_val; rgb[2]=p; break;
            case 1: rgb[0]=q; rgb[1]=v;     rgb[2]=p; break;
            case 2: rgb[0]=p; rgb[1]=v;     rgb[2]=t_val; break;
            case 3: rgb[0]=p; rgb[1]=q;     rgb[2]=v; break;
            case 4: rgb[0]=t_val; rgb[1]=p; rgb[2]=v; break;
            case 5: rgb[0]=v; rgb[1]=p;     rgb[2]=q; break;
        }
        c.r = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[0], SUT_FP_C(255))));
        c.g = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[1], SUT_FP_C(255))));
        c.b = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[2], SUT_FP_C(255))));
    #endif
    return c;
}

sut_rgb_t sut_rgb_blend_opacity(sut_rgb_t bg, sut_rgb_t fg, sut_real_t opacity) {
    sut_rgb_t c;
    sut_real_t o = sut_clamp(opacity, SUT_FP_C(0), SUT_FP_C(1));
    sut_real_t inv_o = SUT_FP_C(1) - o;
    c.r = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(SUT_REAL_FROM_INT(bg.r), inv_o) + sut_fp_mul(SUT_REAL_FROM_INT(fg.r), o)));
    c.g = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(SUT_REAL_FROM_INT(bg.g), inv_o) + sut_fp_mul(SUT_REAL_FROM_INT(fg.g), o)));
    c.b = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(SUT_REAL_FROM_INT(bg.b), inv_o) + sut_fp_mul(SUT_REAL_FROM_INT(fg.b), o)));
    return c;
}

sut_rgb_t sut_rgb_blend_difference(sut_rgb_t bg, sut_rgb_t fg) {
    sut_rgb_t c;
    c.r = bg.r > fg.r ? bg.r - fg.r : fg.r - bg.r;
    c.g = bg.g > fg.g ? bg.g - fg.g : fg.g - bg.g;
    c.b = bg.b > fg.b ? bg.b - fg.b : fg.b - bg.b;
    return c;
}
```

- [ ] **Step 3: Create color test**

```c
// lib/test/test_color.c
#include <stdio.h>
#include "sut_color.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_hex_conversion() {
    TEST("hex round-trip");
    sut_rgb_t c = sut_rgb_from_hex(0xFF8800);
    CHECK(c.r == 0xFF); CHECK(c.g == 0x88); CHECK(c.b == 0x00);
    CHECK(sut_rgb_to_hex(c) == 0xFF8800);
    PASS(); return 0;
}

static int test_blend_opacity_bounds() {
    TEST("blend opacity: 0% = bg, 100% = fg");
    sut_rgb_t bg = {100, 100, 100};
    sut_rgb_t fg = {200, 200, 200};
    sut_rgb_t r0 = sut_rgb_blend_opacity(bg, fg, SUT_FP_C(0));
    CHECK(r0.r == 100 && r0.g == 100 && r0.b == 100);
    sut_rgb_t r1 = sut_rgb_blend_opacity(bg, fg, SUT_FP_C(1));
    CHECK(r1.r == 200 && r1.g == 200 && r1.b == 200);
    PASS(); return 0;
}

static int test_hsv_red() {
    TEST("hsv(0,100%,100%) = red");
    sut_rgb_t c = sut_rgb_from_hsv(SUT_FP_C(0), SUT_FP_C(1), SUT_FP_C(1));
    CHECK(c.r == 255); CHECK(c.g == 0); CHECK(c.b == 0);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_hex_conversion();
    failed += test_blend_opacity_bounds();
    failed += test_hsv_red();
    printf("Color: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify + commit**

Run: `cd lib/build && make test_color && ./test/test_color`
Expected: PASS

```bash
git add lib/include/sut_color.h lib/src/sut_color.c lib/test/test_color.c
git commit -m "c99-port: add color utilities + test"
```

---

### Task 8: HAL abstraction

**Files:**
- Create: `lib/include/sut_hal.h`
- Create: `lib/src/sut_hal.c`

- [ ] **Step 1: Create lib/include/sut_hal.h**

```c
#ifndef SUT_HAL_H
#define SUT_HAL_H

#include <stdint.h>

typedef uint32_t (*sut_get_tick_t)(void);
typedef void     (*sut_delay_t)(uint32_t ms);

void     sut_hal_set_tick_callback(sut_get_tick_t cb);
void     sut_hal_set_delay_callback(sut_delay_t cb);
uint32_t sut_get_tick(void);
void     sut_delay(uint32_t ms);

#endif
```

- [ ] **Step 2: Create lib/src/sut_hal.c**

```c
#include "sut_hal.h"

static sut_get_tick_t _get_tick = NULL;
static sut_delay_t _delay = NULL;

void sut_hal_set_tick_callback(sut_get_tick_t cb) { _get_tick = cb; }
void sut_hal_set_delay_callback(sut_delay_t cb) { _delay = cb; }

uint32_t sut_get_tick(void) {
    return _get_tick ? _get_tick() : 0;
}

void sut_delay(uint32_t ms) {
    if (_delay) _delay(ms);
}
```

- [ ] **Step 3: Commit**

```bash
git add lib/include/sut_hal.h lib/src/sut_hal.c
git commit -m "c99-port: add HAL abstraction (tick/delay)"
```

---

### Task 9: Ring buffer

**Files:**
- Create: `lib/include/sut_ringbuf.h`
- Create: `lib/src/sut_ringbuf.c`
- Create: `lib/test/test_ringbuf.c`

- [ ] **Step 1: Create lib/include/sut_ringbuf.h**

```c
#ifndef SUT_RINGBUF_H
#define SUT_RINGBUF_H

#include <stdint.h>
#include <stdbool.h>
#include "sut_types.h"
#include "sut_errors.h"
#include "sut_allocator.h"

typedef struct {
    uint8_t*  buffer;
    uint16_t  element_size;
    uint16_t  capacity;
    uint16_t  head;
    uint16_t  tail;
    uint16_t  count;
    uint8_t   overwrite;
} sut_ringbuf_t;

int  sut_ringbuf_init(sut_ringbuf_t* rb, uint16_t element_size, uint16_t capacity);
void sut_ringbuf_deinit(sut_ringbuf_t* rb);
int  sut_ringbuf_push(sut_ringbuf_t* rb, const void* data);
int  sut_ringbuf_pop(sut_ringbuf_t* rb, void* out);
int  sut_ringbuf_peek(sut_ringbuf_t* rb, uint16_t index, void* out);
void sut_ringbuf_clear(sut_ringbuf_t* rb);
int  sut_ringbuf_count(sut_ringbuf_t* rb);

#endif
```

- [ ] **Step 2: Create lib/src/sut_ringbuf.c**

```c
#include "sut_ringbuf.h"
#include <string.h>

int sut_ringbuf_init(sut_ringbuf_t* rb, uint16_t element_size, uint16_t capacity) {
    rb->buffer = (uint8_t*)sut_malloc((size_t)element_size * capacity);
    if (!rb->buffer) return SUT_ERR_NOMEM;
    rb->element_size = element_size;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overwrite = 0;
    return SUT_OK;
}

void sut_ringbuf_deinit(sut_ringbuf_t* rb) {
    sut_free(rb->buffer);
    memset(rb, 0, sizeof(*rb));
}

int sut_ringbuf_push(sut_ringbuf_t* rb, const void* data) {
    if (rb->count >= rb->capacity) {
        if (!rb->overwrite) return SUT_ERR_FULL;
        rb->head = (rb->head + 1) % rb->capacity;
    } else {
        rb->count++;
    }
    memcpy(rb->buffer + rb->tail * rb->element_size, data, rb->element_size);
    rb->tail = (rb->tail + 1) % rb->capacity;
    return SUT_OK;
}

int sut_ringbuf_pop(sut_ringbuf_t* rb, void* out) {
    if (rb->count == 0) return SUT_ERR_EMPTY;
    memcpy(out, rb->buffer + rb->head * rb->element_size, rb->element_size);
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;
    return SUT_OK;
}

int sut_ringbuf_peek(sut_ringbuf_t* rb, uint16_t index, void* out) {
    if (index >= rb->count) return SUT_ERR_INVAL;
    uint16_t pos = (rb->head + index) % rb->capacity;
    memcpy(out, rb->buffer + pos * rb->element_size, rb->element_size);
    return SUT_OK;
}

void sut_ringbuf_clear(sut_ringbuf_t* rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

int sut_ringbuf_count(sut_ringbuf_t* rb) {
    return (int)rb->count;
}
```

- [ ] **Step 3: Create ring buffer test**

```c
// lib/test/test_ringbuf.c
#include <stdio.h>
#include "sut_ringbuf.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_push_pop() {
    TEST("push 3, pop 3");
    sut_ringbuf_t rb;
    CHECK(sut_ringbuf_init(&rb, sizeof(int), 5) == SUT_OK);
    int v;
    for (int i = 0; i < 3; i++) { v = i * 10; CHECK(sut_ringbuf_push(&rb, &v) == SUT_OK); }
    CHECK(sut_ringbuf_count(&rb) == 3);
    CHECK(sut_ringbuf_pop(&rb, &v) == SUT_OK); CHECK(v == 0);
    CHECK(sut_ringbuf_pop(&rb, &v) == SUT_OK); CHECK(v == 10);
    CHECK(sut_ringbuf_pop(&rb, &v) == SUT_OK); CHECK(v == 20);
    CHECK(sut_ringbuf_pop(&rb, &v) == SUT_ERR_EMPTY);
    sut_ringbuf_deinit(&rb);
    PASS(); return 0;
}

static int test_overwrite() {
    TEST("overwrite enabled: push 7 into capacity 5");
    sut_ringbuf_t rb;
    CHECK(sut_ringbuf_init(&rb, sizeof(int), 5) == SUT_OK);
    rb.overwrite = 1;
    int v;
    for (int i = 0; i < 7; i++) { v = i; CHECK(sut_ringbuf_push(&rb, &v) == SUT_OK); }
    CHECK(sut_ringbuf_count(&rb) == 5);
    CHECK(sut_ringbuf_pop(&rb, &v) == SUT_OK); CHECK(v == 2);
    sut_ringbuf_deinit(&rb);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_push_pop();
    failed += test_overwrite();
    printf("RingBuf: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify + commit**

Run: `cd lib/build && make test_ringbuf && ./test/test_ringbuf`
Expected: PASS

```bash
git add lib/include/sut_ringbuf.h lib/src/sut_ringbuf.c lib/test/test_ringbuf.c
git commit -m "c99-port: add ring buffer + test"
```

---

### Task 10: Signal (observer)

**Files:**
- Create: `lib/include/sut_signal.h`
- Create: `lib/src/sut_signal.c`
- Create: `lib/test/test_signal.c`

- [ ] **Step 1: Create lib/include/sut_signal.h**

```c
#ifndef SUT_SIGNAL_H
#define SUT_SIGNAL_H

#include <stdint.h>
#include "sut_types.h"
#include "sut_errors.h"

typedef void (*sut_slot_fn)(void* ctx, void* args);

typedef struct {
    sut_slot_fn callback;
    void*       ctx;
} sut_slot_t;

typedef struct {
    sut_slot_t* slots;
    uint16_t    capacity;
    uint16_t    count;
} sut_signal_t;

void sut_signal_init(sut_signal_t* sig, uint16_t capacity);
void sut_signal_deinit(sut_signal_t* sig);
int  sut_signal_connect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_disconnect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_emit(sut_signal_t* sig, void* args);

#endif
```

- [ ] **Step 2: Create lib/src/sut_signal.c**

```c
#include "sut_signal.h"
#include <string.h>

void sut_signal_init(sut_signal_t* sig, uint16_t capacity) {
    sig->slots = (sut_slot_t*)sut_malloc(capacity * sizeof(sut_slot_t));
    sig->capacity = sig->slots ? capacity : 0;
    sig->count = 0;
}

void sut_signal_deinit(sut_signal_t* sig) {
    sut_free(sig->slots);
    memset(sig, 0, sizeof(*sig));
}

int sut_signal_connect(sut_signal_t* sig, sut_slot_fn cb, void* ctx) {
    if (sig->count >= sig->capacity) return SUT_ERR_FULL;
    sig->slots[sig->count].callback = cb;
    sig->slots[sig->count].ctx = ctx;
    sig->count++;
    return SUT_OK;
}

void sut_signal_disconnect(sut_signal_t* sig, sut_slot_fn cb, void* ctx) {
    for (uint16_t i = 0; i < sig->count; i++) {
        if (sig->slots[i].callback == cb && sig->slots[i].ctx == ctx) {
            sig->slots[i] = sig->slots[sig->count - 1];
            sig->count--;
            return;
        }
    }
}

void sut_signal_emit(sut_signal_t* sig, void* args) {
    for (uint16_t i = 0; i < sig->count; i++) {
        sig->slots[i].callback(sig->slots[i].ctx, args);
    }
}
```

- [ ] **Step 3: Create signal test**

```c
// lib/test/test_signal.c
#include <stdio.h>
#include "sut_signal.h"

static int calls = 0;
static void handler1(void* ctx, void* args) { calls++; *(int*)args += 1; }
static void handler2(void* ctx, void* args) { calls++; *(int*)args += 2; }

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
#define PASS() do { tests_passed++; printf("PASS\n"); } while(0)
#define CHECK(c) do { if (!(c)) { printf("FAIL at %d\n", __LINE__); return 1; } } while(0)

static int test_signal_emit() {
    TEST("connect 2 slots, emit");
    sut_signal_t sig;
    sut_signal_init(&sig, 5);
    sut_signal_connect(&sig, handler1, NULL);
    sut_signal_connect(&sig, handler2, NULL);
    calls = 0;
    int val = 0;
    sut_signal_emit(&sig, &val);
    CHECK(val == 3);
    CHECK(calls == 2);
    sut_signal_deinit(&sig);
    PASS(); return 0;
}

static int test_signal_disconnect() {
    TEST("connect, disconnect, emit");
    sut_signal_t sig;
    sut_signal_init(&sig, 5);
    sut_signal_connect(&sig, handler1, NULL);
    sut_signal_connect(&sig, handler2, NULL);
    sut_signal_disconnect(&sig, handler1, NULL);
    calls = 0;
    int val = 0;
    sut_signal_emit(&sig, &val);
    CHECK(val == 2);
    CHECK(calls == 1);
    sut_signal_deinit(&sig);
    PASS(); return 0;
}

int main() {
    int failed = 0;
    failed += test_signal_emit();
    failed += test_signal_disconnect();
    printf("Signal: %d/%d passed\n", tests_passed, tests_run);
    return failed;
}
```

- [ ] **Step 4: Verify + commit**

```bash
git add lib/include/sut_signal.h lib/src/sut_signal.c lib/test/test_signal.c
git commit -m "c99-port: add signal/slot observer + test"
```

---

### Task 11: Object pool

**Files:**
- Create: `lib/include/sut_pool.h`
- Create: `lib/src/sut_pool.c`
- Create: `lib/test/test_pool.c`

- [ ] **Step 1: Create lib/include/sut_pool.h**

```c
#ifndef SUT_POOL_H
#define SUT_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include "sut_types.h"
#include "sut_errors.h"

typedef void (*sut_pool_ctor_fn)(void* obj, void* ctx);
typedef void (*sut_pool_dtor_fn)(void* obj);

typedef struct {
    uint8_t*  memory;
    int*      free_list;
    uint8_t*  used_flags;
    uint16_t  object_size;
    uint16_t  capacity;
    int16_t   free_count;
} sut_pool_t;

int   sut_pool_init(sut_pool_t* pool, uint16_t object_size, uint16_t capacity);
void  sut_pool_deinit(sut_pool_t* pool);
void* sut_pool_acquire(sut_pool_t* pool, sut_pool_ctor_fn ctor, void* ctx);
void  sut_pool_release(sut_pool_t* pool, void* obj);
int   sut_pool_available(sut_pool_t* pool);

#endif
```

- [ ] **Step 2: Create lib/src/sut_pool.c**

```c
#include "sut_pool.h"
#include <string.h>

int sut_pool_init(sut_pool_t* pool, uint16_t object_size, uint16_t capacity) {
    pool->memory = (uint8_t*)sut_malloc((size_t)object_size * capacity);
    pool->free_list = (int*)sut_malloc((size_t)capacity * sizeof(int));
    pool->used_flags = (uint8_t*)sut_malloc(capacity);
    if (!pool->memory || !pool->free_list || !pool->used_flags) {
        sut_free(pool->memory); sut_free(pool->free_list); sut_free(pool->used_flags);
        return SUT_ERR_NOMEM;
    }
    pool->object_size = object_size;
    pool->capacity = capacity;
    pool->free_count = (int16_t)capacity;
    for (uint16_t i = 0; i < capacity; i++) {
        pool->free_list[i] = (int)i;
        pool->used_flags[i] = 0;
    }
    return SUT_OK;
}

void sut_pool_deinit(sut_pool_t* pool) {
    sut_free(pool->memory);
    sut_free(pool->free_list);
    sut_free(pool->used_flags);
    memset(pool, 0, sizeof(*pool));
}

void* sut_pool_acquire(sut_pool_t* pool, sut_pool_ctor_fn ctor, void* ctx) {
    if (pool->free_count <= 0) return NULL;
    int index = pool->free_list[pool->free_count - 1];
    pool->free_count--;
    pool->used_flags[index] = 1;
    void* obj = pool->memory + (size_t)index * pool->object_size;
    if (ctor) ctor(obj, ctx);
    return obj;
}

void sut_pool_release(sut_pool_t* pool, void* obj) {
    uintptr_t offset = (uintptr_t)((uint8_t*)obj - pool->memory);
    if (offset % pool->object_size != 0) return;
    int index = (int)(offset / pool->object_size);
    if (index < 0 || index >= pool->capacity || !pool->used_flags[index]) return;
    pool->used_flags[index] = 0;
    pool->free_list[pool->free_count] = index;
    pool->free_count++;
}

int sut_pool_available(sut_pool_t* pool) { return pool->free_count; }
```

- [ ] **Step 3: Create pool test**

```c
// lib/test/test_pool.c
#include <stdio.h>
#include "sut_pool.h"

static int tests_run = 0, tests_passed = 0;
#define TEST(n) do { tests_run++; printf("  %s ... ", n);
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
```

- [ ] **Step 4: Verify + commit**

Run: `cd lib/build && make test_pool && ./test/test_pool`
Expected: PASS

```bash
git add lib/include/sut_pool.h lib/src/sut_pool.c lib/test/test_pool.c
git commit -m "c99-port: add object pool + test"
```

---

### Task 12: Verify full build + update umbrella header

- [ ] **Step 1: Full clean build and test**

Run: `cd lib/build && cmake .. && make && ctest --output-on-failure`
Expected: All 9 test targets compile, all tests PASS

- [ ] **Step 2: Final commit**

```bash
git add lib/
git commit -m "c99-port: complete library, all tests passing"
```
