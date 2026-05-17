#include "sut_easing.h"
#include "sut_math.h"

static sut_real_t fp_pow2(sut_real_t t) { return sut_fp_mul(t, t); }
static sut_real_t fp_pow3(sut_real_t t) { return sut_fp_mul(fp_pow2(t), t); }
static sut_real_t fp_pow4(sut_real_t t) { return sut_fp_mul(fp_pow3(t), t); }
static sut_real_t fp_pow5(sut_real_t t) { return sut_fp_mul(fp_pow4(t), t); }

sut_real_t sut_ease_linear(sut_real_t t) {
    return t;
}

sut_real_t sut_ease_quad_in(sut_real_t t) {
    return fp_pow2(t);
}

sut_real_t sut_ease_quad_out(sut_real_t t) {
    return SUT_FP_C(1) - fp_pow2(SUT_FP_C(1) - t);
}

sut_real_t sut_ease_quad_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(fp_pow2(t), SUT_FP_C(2));
    return SUT_FP_C(1) - sut_fp_mul(fp_pow2(SUT_FP_C(1) - t), SUT_FP_C(2));
}

sut_real_t sut_ease_cubic_in(sut_real_t t) {
    return fp_pow3(t);
}

sut_real_t sut_ease_cubic_out(sut_real_t t) {
    return SUT_FP_C(1) - fp_pow3(SUT_FP_C(1) - t);
}

sut_real_t sut_ease_cubic_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(fp_pow3(t), SUT_FP_C(4));
    return SUT_FP_C(1) - sut_fp_mul(fp_pow3(SUT_FP_C(1) - t), SUT_FP_C(4));
}

sut_real_t sut_ease_quart_in(sut_real_t t) {
    return fp_pow4(t);
}

sut_real_t sut_ease_quart_out(sut_real_t t) {
    return SUT_FP_C(1) - fp_pow4(SUT_FP_C(1) - t);
}

sut_real_t sut_ease_quart_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(fp_pow4(t), SUT_FP_C(8));
    return SUT_FP_C(1) - sut_fp_mul(fp_pow4(SUT_FP_C(1) - t), SUT_FP_C(8));
}

sut_real_t sut_ease_quint_in(sut_real_t t) {
    return fp_pow5(t);
}

sut_real_t sut_ease_quint_out(sut_real_t t) {
    return SUT_FP_C(1) - fp_pow5(SUT_FP_C(1) - t);
}

sut_real_t sut_ease_quint_in_out(sut_real_t t) {
    if (t < SUT_FP_C(0.5f))
        return sut_fp_mul(fp_pow5(t), SUT_FP_C(16));
    return SUT_FP_C(1) - sut_fp_mul(fp_pow5(SUT_FP_C(1) - t), SUT_FP_C(16));
}

sut_real_t sut_ease_lut(const sut_real_t* lut, sut_real_t t) {
    if (t <= SUT_FP_C(0)) return lut[0];
    if (t >= SUT_FP_C(1)) return lut[256];
    int idx = (int)(t >> 8);
    sut_real_t frac = (sut_real_t)((t & 0xFF) << 8);
    return lut[idx] + sut_fp_mul(lut[idx + 1] - lut[idx], frac);
}

sut_real_t sut_ease_sine_in(sut_real_t t) { return sut_ease_lut(sut_ease_sine_in_lut, t); }
sut_real_t sut_ease_sine_out(sut_real_t t) { return sut_ease_lut(sut_ease_sine_out_lut, t); }
sut_real_t sut_ease_sine_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_sine_in_out_lut, t); }
sut_real_t sut_ease_expo_in(sut_real_t t) { return sut_ease_lut(sut_ease_expo_in_lut, t); }
sut_real_t sut_ease_expo_out(sut_real_t t) { return sut_ease_lut(sut_ease_expo_out_lut, t); }
sut_real_t sut_ease_expo_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_expo_in_out_lut, t); }
sut_real_t sut_ease_circ_in(sut_real_t t) { return sut_ease_lut(sut_ease_circ_in_lut, t); }
sut_real_t sut_ease_circ_out(sut_real_t t) { return sut_ease_lut(sut_ease_circ_out_lut, t); }
sut_real_t sut_ease_circ_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_circ_in_out_lut, t); }
sut_real_t sut_ease_back_in(sut_real_t t) { return sut_ease_lut(sut_ease_back_in_lut, t); }
sut_real_t sut_ease_back_out(sut_real_t t) { return sut_ease_lut(sut_ease_back_out_lut, t); }
sut_real_t sut_ease_back_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_back_in_out_lut, t); }
sut_real_t sut_ease_elastic_in(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_in_lut, t); }
sut_real_t sut_ease_elastic_out(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_out_lut, t); }
sut_real_t sut_ease_elastic_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_elastic_in_out_lut, t); }
sut_real_t sut_ease_bounce_in(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_in_lut, t); }
sut_real_t sut_ease_bounce_out(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_out_lut, t); }
sut_real_t sut_ease_bounce_in_out(sut_real_t t) { return sut_ease_lut(sut_ease_bounce_in_out_lut, t); }

sut_real_t sut_ease_cubic_bezier(sut_real_t t, sut_real_t x1, sut_real_t y1, sut_real_t x2, sut_real_t y2) {
    sut_real_t low = SUT_FP_C(0), high = SUT_FP_C(1);
    for (int i = 0; i < 8; i++) {
        sut_real_t mid = low + sut_fp_mul(high - low, SUT_FP_C(0.5f));
        sut_real_t q0 = sut_fp_mul(mid, x1);
        sut_real_t q1 = x1 + sut_fp_mul(mid, x2 - x1);
        sut_real_t q2 = x2 + sut_fp_mul(mid, SUT_FP_C(1) - x2);
        sut_real_t r0 = q0 + sut_fp_mul(mid, q1 - q0);
        sut_real_t r1 = q1 + sut_fp_mul(mid, q2 - q1);
        sut_real_t x = r0 + sut_fp_mul(mid, r1 - r0);
        if (x < t) low = mid;
        else high = mid;
    }
    sut_real_t tparam = low + sut_fp_mul(high - low, SUT_FP_C(0.5f));
    sut_real_t yq0 = sut_fp_mul(tparam, y1);
    sut_real_t yq1 = y1 + sut_fp_mul(tparam, y2 - y1);
    sut_real_t yq2 = y2 + sut_fp_mul(tparam, SUT_FP_C(1) - y2);
    sut_real_t yr0 = yq0 + sut_fp_mul(tparam, yq1 - yq0);
    sut_real_t yr1 = yq1 + sut_fp_mul(tparam, yq2 - yq1);
    return yr0 + sut_fp_mul(tparam, yr1 - yr0);
}
