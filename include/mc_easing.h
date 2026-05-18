#ifndef MC_EASING_H
#define MC_EASING_H

#include "mc_types.h"

typedef mc_real_t (*mc_easing_fn_t)(mc_real_t t);

mc_real_t mc_ease_linear(mc_real_t t);
mc_real_t mc_ease_quad_in(mc_real_t t);
mc_real_t mc_ease_quad_out(mc_real_t t);
mc_real_t mc_ease_quad_in_out(mc_real_t t);
mc_real_t mc_ease_cubic_in(mc_real_t t);
mc_real_t mc_ease_cubic_out(mc_real_t t);
mc_real_t mc_ease_cubic_in_out(mc_real_t t);
mc_real_t mc_ease_quart_in(mc_real_t t);
mc_real_t mc_ease_quart_out(mc_real_t t);
mc_real_t mc_ease_quart_in_out(mc_real_t t);
mc_real_t mc_ease_quint_in(mc_real_t t);
mc_real_t mc_ease_quint_out(mc_real_t t);
mc_real_t mc_ease_quint_in_out(mc_real_t t);

#ifndef MC_USE_FLOAT
extern const mc_real_t mc_ease_sine_in_lut[257];
extern const mc_real_t mc_ease_sine_out_lut[257];
extern const mc_real_t mc_ease_sine_in_out_lut[257];
extern const mc_real_t mc_ease_expo_in_lut[257];
extern const mc_real_t mc_ease_expo_out_lut[257];
extern const mc_real_t mc_ease_expo_in_out_lut[257];
extern const mc_real_t mc_ease_circ_in_lut[257];
extern const mc_real_t mc_ease_circ_out_lut[257];
extern const mc_real_t mc_ease_circ_in_out_lut[257];
extern const mc_real_t mc_ease_back_in_lut[257];
extern const mc_real_t mc_ease_back_out_lut[257];
extern const mc_real_t mc_ease_back_in_out_lut[257];
extern const mc_real_t mc_ease_elastic_in_lut[257];
extern const mc_real_t mc_ease_elastic_out_lut[257];
extern const mc_real_t mc_ease_elastic_in_out_lut[257];
extern const mc_real_t mc_ease_bounce_in_lut[257];
extern const mc_real_t mc_ease_bounce_out_lut[257];
extern const mc_real_t mc_ease_bounce_in_out_lut[257];

mc_real_t mc_ease_sine_in(mc_real_t t);
mc_real_t mc_ease_sine_out(mc_real_t t);
mc_real_t mc_ease_sine_in_out(mc_real_t t);
mc_real_t mc_ease_expo_in(mc_real_t t);
mc_real_t mc_ease_expo_out(mc_real_t t);
mc_real_t mc_ease_expo_in_out(mc_real_t t);
mc_real_t mc_ease_circ_in(mc_real_t t);
mc_real_t mc_ease_circ_out(mc_real_t t);
mc_real_t mc_ease_circ_in_out(mc_real_t t);
mc_real_t mc_ease_back_in(mc_real_t t);
mc_real_t mc_ease_back_out(mc_real_t t);
mc_real_t mc_ease_back_in_out(mc_real_t t);
mc_real_t mc_ease_elastic_in(mc_real_t t);
mc_real_t mc_ease_elastic_out(mc_real_t t);
mc_real_t mc_ease_elastic_in_out(mc_real_t t);
mc_real_t mc_ease_bounce_in(mc_real_t t);
mc_real_t mc_ease_bounce_out(mc_real_t t);
mc_real_t mc_ease_bounce_in_out(mc_real_t t);

mc_real_t mc_ease_lut(const mc_real_t* lut, mc_real_t t);
#endif
mc_real_t mc_ease_cubic_bezier(mc_real_t t, mc_real_t x1, mc_real_t y1, mc_real_t x2, mc_real_t y2);

#endif
