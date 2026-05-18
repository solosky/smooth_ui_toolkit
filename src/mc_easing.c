#include "mc_easing.h"
#include "mc_math.h"

static mc_real_t fp_pow2(mc_real_t t) { return mc_fp_mul(t, t); }
static mc_real_t fp_pow3(mc_real_t t) { return mc_fp_mul(fp_pow2(t), t); }
static mc_real_t fp_pow4(mc_real_t t) { return mc_fp_mul(fp_pow3(t), t); }
static mc_real_t fp_pow5(mc_real_t t) { return mc_fp_mul(fp_pow4(t), t); }

mc_real_t mc_ease_linear(mc_real_t t) {
    return t;
}

mc_real_t mc_ease_quad_in(mc_real_t t) {
    return fp_pow2(t);
}

mc_real_t mc_ease_quad_out(mc_real_t t) {
    return MC_FP_C(1) - fp_pow2(MC_FP_C(1) - t);
}

mc_real_t mc_ease_quad_in_out(mc_real_t t) {
    if (t < MC_FP_C(0.5f))
        return mc_fp_mul(fp_pow2(t), MC_FP_C(2));
    return MC_FP_C(1) - mc_fp_mul(fp_pow2(MC_FP_C(1) - t), MC_FP_C(2));
}

mc_real_t mc_ease_cubic_in(mc_real_t t) {
    return fp_pow3(t);
}

mc_real_t mc_ease_cubic_out(mc_real_t t) {
    return MC_FP_C(1) - fp_pow3(MC_FP_C(1) - t);
}

mc_real_t mc_ease_cubic_in_out(mc_real_t t) {
    if (t < MC_FP_C(0.5f))
        return mc_fp_mul(fp_pow3(t), MC_FP_C(4));
    return MC_FP_C(1) - mc_fp_mul(fp_pow3(MC_FP_C(1) - t), MC_FP_C(4));
}

mc_real_t mc_ease_quart_in(mc_real_t t) {
    return fp_pow4(t);
}

mc_real_t mc_ease_quart_out(mc_real_t t) {
    return MC_FP_C(1) - fp_pow4(MC_FP_C(1) - t);
}

mc_real_t mc_ease_quart_in_out(mc_real_t t) {
    if (t < MC_FP_C(0.5f))
        return mc_fp_mul(fp_pow4(t), MC_FP_C(8));
    return MC_FP_C(1) - mc_fp_mul(fp_pow4(MC_FP_C(1) - t), MC_FP_C(8));
}

mc_real_t mc_ease_quint_in(mc_real_t t) {
    return fp_pow5(t);
}

mc_real_t mc_ease_quint_out(mc_real_t t) {
    return MC_FP_C(1) - fp_pow5(MC_FP_C(1) - t);
}

mc_real_t mc_ease_quint_in_out(mc_real_t t) {
    if (t < MC_FP_C(0.5f))
        return mc_fp_mul(fp_pow5(t), MC_FP_C(16));
    return MC_FP_C(1) - mc_fp_mul(fp_pow5(MC_FP_C(1) - t), MC_FP_C(16));
}

#ifndef MC_USE_FLOAT

mc_real_t mc_ease_lut(const mc_real_t* lut, mc_real_t t) {
    if (t <= MC_FP_C(0)) return lut[0];
    if (t >= MC_FP_C(1)) return lut[256];
    int idx = (int)(t >> 8);
    mc_real_t frac = (mc_real_t)((t & 0xFF) << 8);
    return lut[idx] + mc_fp_mul(lut[idx + 1] - lut[idx], frac);
}

mc_real_t mc_ease_sine_in(mc_real_t t) { return mc_ease_lut(mc_ease_sine_in_lut, t); }
mc_real_t mc_ease_sine_out(mc_real_t t) { return mc_ease_lut(mc_ease_sine_out_lut, t); }
mc_real_t mc_ease_sine_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_sine_in_out_lut, t); }
mc_real_t mc_ease_expo_in(mc_real_t t) { return mc_ease_lut(mc_ease_expo_in_lut, t); }
mc_real_t mc_ease_expo_out(mc_real_t t) { return mc_ease_lut(mc_ease_expo_out_lut, t); }
mc_real_t mc_ease_expo_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_expo_in_out_lut, t); }
mc_real_t mc_ease_circ_in(mc_real_t t) { return mc_ease_lut(mc_ease_circ_in_lut, t); }
mc_real_t mc_ease_circ_out(mc_real_t t) { return mc_ease_lut(mc_ease_circ_out_lut, t); }
mc_real_t mc_ease_circ_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_circ_in_out_lut, t); }
mc_real_t mc_ease_back_in(mc_real_t t) { return mc_ease_lut(mc_ease_back_in_lut, t); }
mc_real_t mc_ease_back_out(mc_real_t t) { return mc_ease_lut(mc_ease_back_out_lut, t); }
mc_real_t mc_ease_back_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_back_in_out_lut, t); }
mc_real_t mc_ease_elastic_in(mc_real_t t) { return mc_ease_lut(mc_ease_elastic_in_lut, t); }
mc_real_t mc_ease_elastic_out(mc_real_t t) { return mc_ease_lut(mc_ease_elastic_out_lut, t); }
mc_real_t mc_ease_elastic_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_elastic_in_out_lut, t); }
mc_real_t mc_ease_bounce_in(mc_real_t t) { return mc_ease_lut(mc_ease_bounce_in_lut, t); }
mc_real_t mc_ease_bounce_out(mc_real_t t) { return mc_ease_lut(mc_ease_bounce_out_lut, t); }
mc_real_t mc_ease_bounce_in_out(mc_real_t t) { return mc_ease_lut(mc_ease_bounce_in_out_lut, t); }

#endif // MC_USE_FLOAT

mc_real_t mc_ease_cubic_bezier(mc_real_t t, mc_real_t x1, mc_real_t y1, mc_real_t x2, mc_real_t y2) {
    mc_real_t low = MC_FP_C(0), high = MC_FP_C(1);
    for (int i = 0; i < 8; i++) {
        mc_real_t mid = low + mc_fp_mul(high - low, MC_FP_C(0.5f));
        mc_real_t q0 = mc_fp_mul(mid, x1);
        mc_real_t q1 = x1 + mc_fp_mul(mid, x2 - x1);
        mc_real_t q2 = x2 + mc_fp_mul(mid, MC_FP_C(1) - x2);
        mc_real_t r0 = q0 + mc_fp_mul(mid, q1 - q0);
        mc_real_t r1 = q1 + mc_fp_mul(mid, q2 - q1);
        mc_real_t x = r0 + mc_fp_mul(mid, r1 - r0);
        if (x < t) low = mid;
        else high = mid;
    }
    mc_real_t tparam = low + mc_fp_mul(high - low, MC_FP_C(0.5f));
    mc_real_t yq0 = mc_fp_mul(tparam, y1);
    mc_real_t yq1 = y1 + mc_fp_mul(tparam, y2 - y1);
    mc_real_t yq2 = y2 + mc_fp_mul(tparam, MC_FP_C(1) - y2);
    mc_real_t yr0 = yq0 + mc_fp_mul(tparam, yq1 - yq0);
    mc_real_t yr1 = yq1 + mc_fp_mul(tparam, yq2 - yq1);
    return yr0 + mc_fp_mul(tparam, yr1 - yr0);
}
