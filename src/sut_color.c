#include "sut_color.h"
#include "sut_math.h"

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
    sut_rgb_t c = {0, 0, 0};
    sut_real_t hf = sut_fp_div(h, SUT_FP_C(60));
    int hi = SUT_REAL_TO_INT(hf) % 6;
    if (hi < 0) hi += 6;
    sut_real_t f = hf - SUT_REAL_TO_INT(hf) * SUT_FP_SCALE;
    sut_real_t p = sut_fp_mul(v, SUT_FP_C(1) - s);
    sut_real_t q = sut_fp_mul(v, SUT_FP_C(1) - sut_fp_mul(s, f));
    sut_real_t t = sut_fp_mul(v, SUT_FP_C(1) - sut_fp_mul(s, SUT_FP_C(1) - f));
    sut_real_t rgb[3] = {0};
    switch (hi) {
        case 0: rgb[0]=v; rgb[1]=t; rgb[2]=p; break;
        case 1: rgb[0]=q; rgb[1]=v; rgb[2]=p; break;
        case 2: rgb[0]=p; rgb[1]=v; rgb[2]=t; break;
        case 3: rgb[0]=p; rgb[1]=q; rgb[2]=v; break;
        case 4: rgb[0]=t; rgb[1]=p; rgb[2]=v; break;
        case 5: rgb[0]=v; rgb[1]=p; rgb[2]=q; break;
    }
    c.r = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[0], SUT_FP_C(255))));
    c.g = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[1], SUT_FP_C(255))));
    c.b = (uint8_t)(SUT_REAL_TO_INT(sut_fp_mul(rgb[2], SUT_FP_C(255))));
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
