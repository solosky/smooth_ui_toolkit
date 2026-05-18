#ifndef MC_MATH_H
#define MC_MATH_H

#include <stdbool.h>
#include "mc_types.h"

mc_real_t mc_clamp(mc_real_t v, mc_real_t min, mc_real_t max);
mc_real_t mc_lerp(mc_real_t a, mc_real_t b, mc_real_t t);
mc_real_t mc_map(mc_real_t v, mc_real_t a1, mc_real_t b1, mc_real_t a2, mc_real_t b2);

mc_real_t mc_fp_mul(mc_real_t a, mc_real_t b);
mc_real_t mc_fp_div(mc_real_t a, mc_real_t b);
mc_real_t mc_fp_sqrt(mc_real_t v);

typedef struct { mc_real_t x, y; } mc_vec2_t;
mc_vec2_t mc_vec2(mc_real_t x, mc_real_t y);
mc_vec2_t mc_vec2_add(mc_vec2_t a, mc_vec2_t b);
mc_vec2_t mc_vec2_sub(mc_vec2_t a, mc_vec2_t b);
mc_vec2_t mc_vec2_mul(mc_vec2_t a, mc_real_t s);
mc_real_t mc_vec2_dot(mc_vec2_t a, mc_vec2_t b);
mc_real_t mc_vec2_length(mc_vec2_t a);
mc_vec2_t mc_vec2_normalize(mc_vec2_t a);
mc_vec2_t mc_vec2_lerp(mc_vec2_t a, mc_vec2_t b, mc_real_t t);

typedef struct { mc_real_t x, y, z, w; } mc_vec4_t;
mc_vec4_t mc_vec4(mc_real_t x, mc_real_t y, mc_real_t z, mc_real_t w);
mc_vec4_t mc_vec4_add(mc_vec4_t a, mc_vec4_t b);
mc_vec4_t mc_vec4_sub(mc_vec4_t a, mc_vec4_t b);
mc_vec4_t mc_vec4_mul(mc_vec4_t a, mc_real_t s);
mc_real_t mc_vec4_dot(mc_vec4_t a, mc_vec4_t b);
mc_vec4_t mc_vec4_lerp(mc_vec4_t a, mc_vec4_t b, mc_real_t t);

typedef struct { mc_real_t x, y, w, h; } mc_rect_t;
bool mc_rect_contains(mc_rect_t r, mc_vec2_t p);
bool mc_rect_overlaps(mc_rect_t a, mc_rect_t b);

#endif
