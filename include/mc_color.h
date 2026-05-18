#ifndef MC_COLOR_H
#define MC_COLOR_H

#include <stdint.h>
#include "mc_types.h"

typedef struct { uint8_t r, g, b; } mc_rgb_t;
typedef struct { uint8_t r, g, b, a; } mc_rgba_t;

mc_rgb_t  mc_rgb_from_hex(uint32_t hex);
uint32_t   mc_rgb_to_hex(mc_rgb_t c);
mc_rgb_t  mc_rgb_from_hsv(mc_real_t h, mc_real_t s, mc_real_t v);
mc_rgb_t  mc_rgb_blend_opacity(mc_rgb_t bg, mc_rgb_t fg, mc_real_t opacity);
mc_rgb_t  mc_rgb_blend_difference(mc_rgb_t bg, mc_rgb_t fg);

#endif
