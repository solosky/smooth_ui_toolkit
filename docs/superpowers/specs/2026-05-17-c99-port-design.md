# Smooth UI Toolkit — C99 Port Design

## Motivation

Target embedded platforms with only C99 support (no C++ compiler). The original C++17 codebase (~15,000 lines, 77 files) uses classes, templates, inheritance, virtual functions, and STL, making it unusable on such platforms.

## Approach

**C language redesign** — not line-by-line translation. The C API is designed for idiomatic C usage while preserving the original animation engine's capabilities.

## Naming Convention

All public API uses `sut_` prefix (Smooth UI Toolkit):

| C++ | C |
|-----|---|
| `smooth_ui_toolkit::ease::linear` | `sut_ease_linear` |
| `smooth_ui_toolkit::Vector2` | `sut_vec2_t` |
| `smooth_ui_toolkit::Animate` | `sut_animate_t` |

## Module Layout

```
include/
  sut_math.h        — clamp, lerp, map, Vector2, Vector4, Rect
  sut_easing.h      — 30+ easing functions + cubic bezier
  sut_spring.h      — spring physics parameters + evaluator
  sut_color.h       — RGB888, hex conversion, blending
  sut_animate.h     — value/vec2/vec4 animation engine
  sut_sequence.h    — animation sequence
  sut_hal.h         — tick/delay abstraction
  sut_ringbuf.h     — ring buffer
  sut_signal.h      — observer pattern
  sut_pool.h        — object pool
  sut_allocator.h   — memory allocator interface
src/
  (corresponding .c files)
examples/
  (ported examples)
```

## Type Convention

All integer types use `<stdint.h>` fixed-width types (`uint8_t`, `uint16_t`, `uint32_t`, `int16_t`, `int32_t`). `bool` from `<stdbool.h>`. These guarantee consistent memory layout across platforms.

## Fixed-Point Convention

All real-number values use Q16.16 fixed-point (`int32_t`) to avoid software float overhead on MCUs without FPU.

```c
typedef int32_t sut_real_t;
#define SUT_FP_SCALE 65536
#define SUT_FP_C(v)  ((sut_real_t)((v) * SUT_FP_SCALE))
```

**Properties:**
- Range: [-32768, +32767] (ample for animation values)
- Resolution: 1/65536 ≈ 0.000015 (exceeds float precision for values near 0)
- Addition/subtraction: single `+`/`-` instruction
- Multiplication: `(int64_t)a * b >> 16` (needs 64-bit intermediate to avoid overflow)
- Division: `(int64_t)a << 16 / b`
- No `sin`/`cos`/`exp`/`sqrt` in libm — all implemented with integer algorithms

## Memory Allocator

Global allocator, set once at startup via `sut_allocator_set()`. Defaults to `malloc`/`free` if not set.

```c
typedef struct {
    void* (*alloc)(size_t size, void* ctx);
    void  (*free)(void* ptr, void* ctx);
    void* ctx;
} sut_allocator_t;

void sut_allocator_set(sut_allocator_t* alloc);
```

Internal helpers `sut_malloc()` / `sut_free()` used by all modules that need dynamic allocation.

## Error Codes

```c
#define SUT_OK           0
#define SUT_ERR_NOMEM   -1
#define SUT_ERR_FULL    -2
#define SUT_ERR_EMPTY   -3
#define SUT_ERR_INVAL   -4
```

## Module: Animation Engine

### Value Animation

```c
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

    sut_real_t velocity;   // spring state (unused in easing mode)

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
```

### Vector2 Animation

```c
typedef struct {
    sut_vec2_t from;
    sut_vec2_t to;
    sut_vec2_t current;
    sut_real_t elapsed;
    sut_real_t delay;

    sut_vec2_t velocity;   // spring state

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
```

Vector4 animation mirrors Vector2 with `sut_vec4_t` fields.

### Animation Sequence

```c
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
} sut_sequence_t;

int  sut_sequence_init(sut_sequence_t* s);
void sut_sequence_deinit(sut_sequence_t* s);
int  sut_sequence_push(sut_sequence_t* s, sut_animate_step_t step);
bool sut_sequence_update(sut_sequence_t* s, sut_real_t dt);
```

Steps array is dynamically allocated via the global allocator. Fixed-step animations that advance through the list each cycle.

## Module: Easing Functions

```c
typedef sut_real_t (*sut_easing_fn_t)(sut_real_t t);

// Simple polynomial easings — computed in fixed-point
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

// Complex easings — pre-computed 256-entry LUTs in flash
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

// Wrapper: t [0, SUT_FP_SCALE], idx = t >> 8, linear interpolate LUT[idx] & LUT[idx+1]
sut_real_t sut_ease_lut(const sut_real_t* lut, sut_real_t t);

// Cubic bezier — computed via De Casteljau in fixed-point
sut_real_t sut_ease_cubic_bezier(sut_real_t t, sut_real_t x1, sut_real_t y1, sut_real_t x2, sut_real_t y2);
```

**Implementation strategy:**
- Simple polynomials (linear ~ quint): fixed-point evaluation, `((int64_t)a * b) >> 16` intermediates
- Complex functions (sine, expo, circ, back, elastic, bounce): single `sut_ease_lut()` call per function
- Each LUT: 257 × 4 = 1028 bytes in flash, reads only, no runtime computation
- Cubic bezier: De Casteljau algorithm with fixed-point math

## Module: Spring Physics

Spring uses **Euler integration** per frame (not analytic solution) — no `sin`/`cos`/`exp` needed:

```c
typedef struct {
    sut_real_t stiffness;
    sut_real_t damping;
    sut_real_t mass;
} sut_spring_params_t;

// Spring state stored inside sut_animate_t
typedef struct {
    sut_real_t velocity;
} sut_spring_state_t;

// Per-frame Euler integration
// force = -k*x - c*v;  a = force/m;  v += a*dt;  x += v*dt
void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt);
```

This replaces the analytic spring solver with a fixed-timestep simulation. Each animation update call advances the spring by `dt`. No pre-computation needed, responds to runtime target changes.

## Module: Math Utilities

```c
sut_real_t sut_clamp(sut_real_t v, sut_real_t min, sut_real_t max);
sut_real_t sut_lerp(sut_real_t a, sut_real_t b, sut_real_t t);
sut_real_t sut_map(sut_real_t v, sut_real_t a1, sut_real_t b1, sut_real_t a2, sut_real_t b2);

// Fixed-point arithmetic helpers
sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b);  // ((int64_t)a * b) >> 16
sut_real_t sut_fp_div(sut_real_t a, sut_real_t b);  // (int64_t)a << 16 / b
sut_real_t sut_fp_sqrt(sut_real_t v);                // Newton iteration

// Vector2
typedef struct { sut_real_t x, y; } sut_vec2_t;
sut_vec2_t sut_vec2(sut_real_t x, sut_real_t y);
sut_vec2_t sut_vec2_add(sut_vec2_t a, sut_vec2_t b);
sut_vec2_t sut_vec2_sub(sut_vec2_t a, sut_vec2_t b);
sut_vec2_t sut_vec2_mul(sut_vec2_t a, sut_real_t s);
sut_real_t sut_vec2_dot(sut_vec2_t a, sut_vec2_t b);
sut_real_t sut_vec2_length(sut_vec2_t a);
sut_vec2_t sut_vec2_normalize(sut_vec2_t a);
sut_vec2_t sut_vec2_lerp(sut_vec2_t a, sut_vec2_t b, sut_real_t t);

// Vector4
typedef struct { sut_real_t x, y, z, w; } sut_vec4_t;
// ... same operations as vec2

// Rect
typedef struct { sut_real_t x, y, w, h; } sut_rect_t;
bool sut_rect_contains(sut_rect_t r, sut_vec2_t p);
bool sut_rect_overlaps(sut_rect_t a, sut_rect_t b);
```

## Module: Color

```c
typedef struct { uint8_t r, g, b; } sut_rgb_t;
typedef struct { uint8_t r, g, b, a; } sut_rgba_t;

sut_rgb_t  sut_rgb_from_hex(uint32_t hex);
uint32_t   sut_rgb_to_hex(sut_rgb_t c);
sut_rgb_t  sut_rgb_from_hsv(sut_real_t h, sut_real_t s, sut_real_t v);
sut_rgb_t  sut_rgb_blend_opacity(sut_rgb_t bg, sut_rgb_t fg, sut_real_t opacity);
sut_rgb_t  sut_rgb_blend_difference(sut_rgb_t bg, sut_rgb_t fg);
```

Color animation handled via `sut_vec3_animate_t` (RGB as vec3).

## Module: HAL

```c
typedef uint32_t (*sut_get_tick_t)(void);
typedef void     (*sut_delay_t)(uint32_t ms);

void     sut_hal_set_tick_callback(sut_get_tick_t cb);
void     sut_hal_set_delay_callback(sut_delay_t cb);
uint32_t sut_get_tick(void);
void     sut_delay(uint32_t ms);
```

Global function pointers, user sets at startup. No default implementation.

## Module: Ring Buffer

```c
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
```

## Module: Signal (Observer)

```c
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

void sut_signal_init(sut_signal_t* sig);
void sut_signal_deinit(sut_signal_t* sig);
int  sut_signal_connect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_disconnect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_emit(sut_signal_t* sig, void* args);
```

Not thread-safe (delegated to upper layer).

## Module: Object Pool

```c
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
void  sut_pool_foreach(sut_pool_t* pool, void (*fn)(void* obj, void* ctx), void* ctx);
```

## Dropped from Original

| Module | Reason |
|--------|--------|
| LVGL C++ wrappers (`src/lvgl/lvgl_cpp/`) | LVGL itself is C; wrapper not needed |
| Number Flow (`src/lvgl/number_flow/`) | LVGL-dependent, not applicable |
| fpm fixed-point math (`src/tools/fpm/`) | Heavy template code; platforms with no FPU can add their own fixed-point as needed |
| Games framework (`src/games/`) | ECS-like pattern in C would diverge significantly; deferred |
| Select menu widgets (`src/widget/`) | LVGL-dependent; deferred or user reimplements with C LVGL API |

## Testing

Use a simple test runner (no framework). Each module gets a `test_*.c` that exercises its API. CTest integration via CMake.

## Build System

CMake-based, with a `C_STANDARD 99` flag. A basic `CMakeLists.txt` compiles all `src/` into a static library `libsut.a`. Examples built conditionally.
