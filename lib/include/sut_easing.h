#ifndef SUT_EASING_H
#define SUT_EASING_H

#include "sut_types.h"

typedef sut_real_t (*sut_easing_fn_t)(sut_real_t t);

sut_real_t sut_ease_linear(sut_real_t t);
sut_real_t sut_ease_quad_in(sut_real_t t);
sut_real_t sut_ease_quad_out(sut_real_t t);
sut_real_t sut_ease_quad_in_out(sut_real_t t);
sut_real_t sut_ease_cubic_in(sut_real_t t);
sut_real_t sut_ease_cubic_out(sut_real_t t);
sut_real_t sut_ease_cubic_in_out(sut_real_t t);
sut_real_t sut_ease_quart_in(sut_real_t t);
sut_real_t sut_ease_quart_out(sut_real_t t);
sut_real_t sut_ease_quart_in_out(sut_real_t t);
sut_real_t sut_ease_quint_in(sut_real_t t);
sut_real_t sut_ease_quint_out(sut_real_t t);
sut_real_t sut_ease_quint_in_out(sut_real_t t);

extern const sut_real_t sut_ease_sine_in_lut[257];
extern const sut_real_t sut_ease_sine_out_lut[257];
extern const sut_real_t sut_ease_sine_in_out_lut[257];
extern const sut_real_t sut_ease_expo_in_lut[257];
extern const sut_real_t sut_ease_expo_out_lut[257];
extern const sut_real_t sut_ease_expo_in_out_lut[257];
extern const sut_real_t sut_ease_circ_in_lut[257];
extern const sut_real_t sut_ease_circ_out_lut[257];
extern const sut_real_t sut_ease_circ_in_out_lut[257];
extern const sut_real_t sut_ease_back_in_lut[257];
extern const sut_real_t sut_ease_back_out_lut[257];
extern const sut_real_t sut_ease_back_in_out_lut[257];
extern const sut_real_t sut_ease_elastic_in_lut[257];
extern const sut_real_t sut_ease_elastic_out_lut[257];
extern const sut_real_t sut_ease_elastic_in_out_lut[257];
extern const sut_real_t sut_ease_bounce_in_lut[257];
extern const sut_real_t sut_ease_bounce_out_lut[257];
extern const sut_real_t sut_ease_bounce_in_out_lut[257];

sut_real_t sut_ease_sine_in(sut_real_t t);
sut_real_t sut_ease_sine_out(sut_real_t t);
sut_real_t sut_ease_sine_in_out(sut_real_t t);
sut_real_t sut_ease_expo_in(sut_real_t t);
sut_real_t sut_ease_expo_out(sut_real_t t);
sut_real_t sut_ease_expo_in_out(sut_real_t t);
sut_real_t sut_ease_circ_in(sut_real_t t);
sut_real_t sut_ease_circ_out(sut_real_t t);
sut_real_t sut_ease_circ_in_out(sut_real_t t);
sut_real_t sut_ease_back_in(sut_real_t t);
sut_real_t sut_ease_back_out(sut_real_t t);
sut_real_t sut_ease_back_in_out(sut_real_t t);
sut_real_t sut_ease_elastic_in(sut_real_t t);
sut_real_t sut_ease_elastic_out(sut_real_t t);
sut_real_t sut_ease_elastic_in_out(sut_real_t t);
sut_real_t sut_ease_bounce_in(sut_real_t t);
sut_real_t sut_ease_bounce_out(sut_real_t t);
sut_real_t sut_ease_bounce_in_out(sut_real_t t);

sut_real_t sut_ease_lut(const sut_real_t* lut, sut_real_t t);
sut_real_t sut_ease_cubic_bezier(sut_real_t t, sut_real_t x1, sut_real_t y1, sut_real_t x2, sut_real_t y2);

#endif
