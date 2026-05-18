#include "mc_color.h"
#include "mc_math.h"

mc_rgb_t mc_rgb_from_hex(uint32_t hex) {
    mc_rgb_t c;
    c.r = (hex >> 16) & 0xFF;
    c.g = (hex >> 8) & 0xFF;
    c.b = hex & 0xFF;
    return c;
}

uint32_t mc_rgb_to_hex(mc_rgb_t c) {
    return ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | c.b;
}

mc_rgb_t mc_rgb_from_hsv(mc_real_t h, mc_real_t s, mc_real_t v) {
    mc_rgb_t c = {0, 0, 0};
    mc_real_t hf = mc_fp_div(h, MC_FP_C(60));
    int hi = MC_REAL_TO_INT(hf) % 6;
    if (hi < 0) hi += 6;
    mc_real_t f = hf - MC_REAL_TO_INT(hf) * MC_FP_SCALE;
    mc_real_t p = mc_fp_mul(v, MC_FP_C(1) - s);
    mc_real_t q = mc_fp_mul(v, MC_FP_C(1) - mc_fp_mul(s, f));
    mc_real_t t = mc_fp_mul(v, MC_FP_C(1) - mc_fp_mul(s, MC_FP_C(1) - f));
    mc_real_t rgb[3] = {0};
    switch (hi) {
        case 0: rgb[0]=v; rgb[1]=t; rgb[2]=p; break;
        case 1: rgb[0]=q; rgb[1]=v; rgb[2]=p; break;
        case 2: rgb[0]=p; rgb[1]=v; rgb[2]=t; break;
        case 3: rgb[0]=p; rgb[1]=q; rgb[2]=v; break;
        case 4: rgb[0]=t; rgb[1]=p; rgb[2]=v; break;
        case 5: rgb[0]=v; rgb[1]=p; rgb[2]=q; break;
    }
    c.r = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(rgb[0], MC_FP_C(255))));
    c.g = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(rgb[1], MC_FP_C(255))));
    c.b = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(rgb[2], MC_FP_C(255))));
    return c;
}

mc_rgb_t mc_rgb_blend_opacity(mc_rgb_t bg, mc_rgb_t fg, mc_real_t opacity) {
    mc_rgb_t c;
    mc_real_t o = mc_clamp(opacity, MC_FP_C(0), MC_FP_C(1));
    mc_real_t inv_o = MC_FP_C(1) - o;
    c.r = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(MC_REAL_FROM_INT(bg.r), inv_o) + mc_fp_mul(MC_REAL_FROM_INT(fg.r), o)));
    c.g = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(MC_REAL_FROM_INT(bg.g), inv_o) + mc_fp_mul(MC_REAL_FROM_INT(fg.g), o)));
    c.b = (uint8_t)(MC_REAL_TO_INT(mc_fp_mul(MC_REAL_FROM_INT(bg.b), inv_o) + mc_fp_mul(MC_REAL_FROM_INT(fg.b), o)));
    return c;
}

mc_rgb_t mc_rgb_blend_difference(mc_rgb_t bg, mc_rgb_t fg) {
    mc_rgb_t c;
    c.r = bg.r > fg.r ? bg.r - fg.r : fg.r - bg.r;
    c.g = bg.g > fg.g ? bg.g - fg.g : fg.g - bg.g;
    c.b = bg.b > fg.b ? bg.b - fg.b : fg.b - bg.b;
    return c;
}
