#include "mc_spring.h"
#include "mc_math.h"
#include <stdint.h>

void mc_spring_step(mc_real_t* x, mc_spring_state_t* s, mc_spring_params_t* p, mc_real_t target, mc_real_t dt) {
    mc_real_t displacement = *x - target;
    int64_t force = mc_fp_mul(p->stiffness, displacement);
    force = -(force + (int64_t)mc_fp_mul(p->damping, s->velocity));
    if (force > INT32_MAX) force = INT32_MAX;
    if (force < INT32_MIN) force = INT32_MIN;
    mc_real_t accel = mc_fp_div((mc_real_t)force, p->mass);
    int64_t vel = (int64_t)s->velocity + mc_fp_mul(accel, dt);
    if (vel > INT32_MAX) vel = INT32_MAX;
    if (vel < INT32_MIN) vel = INT32_MIN;
    s->velocity = (mc_real_t)vel;
    int64_t pos = (int64_t)*x + mc_fp_mul(s->velocity, dt);
    if (pos > INT32_MAX) pos = INT32_MAX;
    if (pos < INT32_MIN) pos = INT32_MIN;
    *x = (mc_real_t)pos;
}
