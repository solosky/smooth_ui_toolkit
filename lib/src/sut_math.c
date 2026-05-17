#include "sut_math.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef SUT_USE_FLOAT
#include <math.h>
#endif

sut_real_t sut_clamp(sut_real_t v, sut_real_t min, sut_real_t max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

sut_real_t sut_lerp(sut_real_t a, sut_real_t b, sut_real_t t) {
    return a + sut_fp_mul(b - a, t);
}

sut_real_t sut_map(sut_real_t v, sut_real_t a1, sut_real_t b1, sut_real_t a2, sut_real_t b2) {
    return a2 + sut_fp_mul(sut_fp_div(v - a1, b1 - a1), b2 - a2);
}

// ---------------------------------------------------------------------------
// Fixed-point math primitives
// ---------------------------------------------------------------------------

#ifdef SUT_USE_FLOAT

sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b) { return a * b; }
sut_real_t sut_fp_div(sut_real_t a, sut_real_t b) { return a / b; }
sut_real_t sut_fp_sqrt(sut_real_t v) { return sqrtf(v); }

#else

sut_real_t sut_fp_mul(sut_real_t a, sut_real_t b) {
    int64_t raw = (int64_t)a * b;
    if (raw > (int64_t)INT32_MAX * SUT_FP_SCALE)
        return INT32_MAX;
    if (raw < (int64_t)INT32_MIN * SUT_FP_SCALE)
        return INT32_MIN;
    return (sut_real_t)(raw >> 16);
}

sut_real_t sut_fp_div(sut_real_t a, sut_real_t b) {
    if (b == 0) return SUT_FP_C(0);
    int32_t neg = 1;
    uint64_t ua = a < 0 ? -(uint64_t)a : (uint64_t)a;
    uint64_t ub = b < 0 ? -(uint64_t)b : (uint64_t)b;
    if (a < 0) neg = -neg;
    if (b < 0) neg = -neg;
    uint32_t result = (uint32_t)((ua << 16) / ub);
    return (sut_real_t)((int32_t)result * neg);
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

// ---------------------------------------------------------------------------
// vec2
// ---------------------------------------------------------------------------

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
    return sut_fp_sqrt(sut_fp_mul(a.x, a.x) + sut_fp_mul(a.y, a.y));
}

sut_vec2_t sut_vec2_normalize(sut_vec2_t a) {
    sut_real_t len = sut_vec2_length(a);
    if (len == 0) return sut_vec2(0, 0);
    sut_vec2_t r = { sut_fp_div(a.x, len), sut_fp_div(a.y, len) };
    return r;
}

sut_vec2_t sut_vec2_lerp(sut_vec2_t a, sut_vec2_t b, sut_real_t t) {
    sut_vec2_t r;
    r.x = sut_lerp(a.x, b.x, t);
    r.y = sut_lerp(a.y, b.y, t);
    return r;
}

// ---------------------------------------------------------------------------
// vec4
// ---------------------------------------------------------------------------

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
    sut_vec4_t r;
    r.x = sut_lerp(a.x, b.x, t);
    r.y = sut_lerp(a.y, b.y, t);
    r.z = sut_lerp(a.z, b.z, t);
    r.w = sut_lerp(a.w, b.w, t);
    return r;
}

// ---------------------------------------------------------------------------
// rect
// ---------------------------------------------------------------------------

bool sut_rect_contains(sut_rect_t r, sut_vec2_t p) {
    return p.x >= r.x && p.x < (r.x + r.w) &&
           p.y >= r.y && p.y < (r.y + r.h);
}

bool sut_rect_overlaps(sut_rect_t a, sut_rect_t b) {
    return a.x < (b.x + b.w) && (a.x + a.w) > b.x &&
           a.y < (b.y + b.h) && (a.y + a.h) > b.y;
}
