#ifndef SUT_MATH_H
#define SUT_MATH_H

#include <stdbool.h>
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
