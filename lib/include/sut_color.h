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
