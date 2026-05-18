#include "mc_math.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef MC_USE_FLOAT
#include <math.h>
#endif

mc_real_t mc_clamp(mc_real_t v, mc_real_t min, mc_real_t max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

mc_real_t mc_lerp(mc_real_t a, mc_real_t b, mc_real_t t) {
    return a + mc_fp_mul(b - a, t);
}

mc_real_t mc_map(mc_real_t v, mc_real_t a1, mc_real_t b1, mc_real_t a2, mc_real_t b2) {
    return a2 + mc_fp_mul(mc_fp_div(v - a1, b1 - a1), b2 - a2);
}

// ---------------------------------------------------------------------------
// Fixed-point math primitives
// ---------------------------------------------------------------------------

#ifdef MC_USE_FLOAT

mc_real_t mc_fp_mul(mc_real_t a, mc_real_t b) { return a * b; }
mc_real_t mc_fp_div(mc_real_t a, mc_real_t b) { return a / b; }
mc_real_t mc_fp_sqrt(mc_real_t v) { return sqrtf(v); }

#else

mc_real_t mc_fp_mul(mc_real_t a, mc_real_t b) {
    int64_t raw = (int64_t)a * b;
    if (raw > (int64_t)INT32_MAX * MC_FP_SCALE)
        return INT32_MAX;
    if (raw < (int64_t)INT32_MIN * MC_FP_SCALE)
        return INT32_MIN;
    return (mc_real_t)(raw >> 16);
}

mc_real_t mc_fp_div(mc_real_t a, mc_real_t b) {
    if (b == 0) return MC_FP_C(0);
    int32_t neg = 1;
    uint64_t ua = a < 0 ? -(uint64_t)a : (uint64_t)a;
    uint64_t ub = b < 0 ? -(uint64_t)b : (uint64_t)b;
    if (a < 0) neg = -neg;
    if (b < 0) neg = -neg;
    uint32_t result = (uint32_t)((ua << 16) / ub);
    return (mc_real_t)((int32_t)result * neg);
}

mc_real_t mc_fp_sqrt(mc_real_t v) {
    if (v <= 0) return 0;
    uint32_t val = (uint32_t)v;
    uint32_t root = (val + MC_FP_C(1)) >> 1;
    for (int i = 0; i < 8; i++) {
        root = (root + (uint32_t)(((uint64_t)val << 16) / root)) >> 1;
    }
    return (mc_real_t)root;
}

#endif // MC_USE_FLOAT

// ---------------------------------------------------------------------------
// vec2
// ---------------------------------------------------------------------------

mc_vec2_t mc_vec2(mc_real_t x, mc_real_t y) {
    mc_vec2_t r = { x, y };
    return r;
}

mc_vec2_t mc_vec2_add(mc_vec2_t a, mc_vec2_t b) {
    mc_vec2_t r = { a.x + b.x, a.y + b.y };
    return r;
}

mc_vec2_t mc_vec2_sub(mc_vec2_t a, mc_vec2_t b) {
    mc_vec2_t r = { a.x - b.x, a.y - b.y };
    return r;
}

mc_vec2_t mc_vec2_mul(mc_vec2_t a, mc_real_t s) {
    mc_vec2_t r = { mc_fp_mul(a.x, s), mc_fp_mul(a.y, s) };
    return r;
}

mc_real_t mc_vec2_dot(mc_vec2_t a, mc_vec2_t b) {
    return mc_fp_mul(a.x, b.x) + mc_fp_mul(a.y, b.y);
}

mc_real_t mc_vec2_length(mc_vec2_t a) {
    return mc_fp_sqrt(mc_fp_mul(a.x, a.x) + mc_fp_mul(a.y, a.y));
}

mc_vec2_t mc_vec2_normalize(mc_vec2_t a) {
    mc_real_t len = mc_vec2_length(a);
    if (len == 0) return mc_vec2(0, 0);
    mc_vec2_t r = { mc_fp_div(a.x, len), mc_fp_div(a.y, len) };
    return r;
}

mc_vec2_t mc_vec2_lerp(mc_vec2_t a, mc_vec2_t b, mc_real_t t) {
    mc_vec2_t r;
    r.x = mc_lerp(a.x, b.x, t);
    r.y = mc_lerp(a.y, b.y, t);
    return r;
}

// ---------------------------------------------------------------------------
// vec4
// ---------------------------------------------------------------------------

mc_vec4_t mc_vec4(mc_real_t x, mc_real_t y, mc_real_t z, mc_real_t w) {
    mc_vec4_t r = { x, y, z, w };
    return r;
}

mc_vec4_t mc_vec4_add(mc_vec4_t a, mc_vec4_t b) {
    mc_vec4_t r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return r;
}

mc_vec4_t mc_vec4_sub(mc_vec4_t a, mc_vec4_t b) {
    mc_vec4_t r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return r;
}

mc_vec4_t mc_vec4_mul(mc_vec4_t a, mc_real_t s) {
    mc_vec4_t r = { mc_fp_mul(a.x, s), mc_fp_mul(a.y, s), mc_fp_mul(a.z, s), mc_fp_mul(a.w, s) };
    return r;
}

mc_real_t mc_vec4_dot(mc_vec4_t a, mc_vec4_t b) {
    return mc_fp_mul(a.x, b.x) + mc_fp_mul(a.y, b.y) + mc_fp_mul(a.z, b.z) + mc_fp_mul(a.w, b.w);
}

mc_vec4_t mc_vec4_lerp(mc_vec4_t a, mc_vec4_t b, mc_real_t t) {
    mc_vec4_t r;
    r.x = mc_lerp(a.x, b.x, t);
    r.y = mc_lerp(a.y, b.y, t);
    r.z = mc_lerp(a.z, b.z, t);
    r.w = mc_lerp(a.w, b.w, t);
    return r;
}

// ---------------------------------------------------------------------------
// rect
// ---------------------------------------------------------------------------

bool mc_rect_contains(mc_rect_t r, mc_vec2_t p) {
    return p.x >= r.x && p.x < (r.x + r.w) &&
           p.y >= r.y && p.y < (r.y + r.h);
}

bool mc_rect_overlaps(mc_rect_t a, mc_rect_t b) {
    return a.x < (b.x + b.w) && (a.x + a.w) > b.x &&
           a.y < (b.y + b.h) && (a.y + a.h) > b.y;
}
