#include "sut_spring.h"
#include "sut_math.h"
#include <stdint.h>

void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt) {
    sut_real_t displacement = *x - target;
    int64_t force = sut_fp_mul(p->stiffness, displacement);
    force = -(force + (int64_t)sut_fp_mul(p->damping, s->velocity));
    if (force > INT32_MAX) force = INT32_MAX;
    if (force < INT32_MIN) force = INT32_MIN;
    sut_real_t accel = sut_fp_div((sut_real_t)force, p->mass);
    int64_t vel = (int64_t)s->velocity + sut_fp_mul(accel, dt);
    if (vel > INT32_MAX) vel = INT32_MAX;
    if (vel < INT32_MIN) vel = INT32_MIN;
    s->velocity = (sut_real_t)vel;
    int64_t pos = (int64_t)*x + sut_fp_mul(s->velocity, dt);
    if (pos > INT32_MAX) pos = INT32_MAX;
    if (pos < INT32_MIN) pos = INT32_MIN;
    *x = (sut_real_t)pos;
}
