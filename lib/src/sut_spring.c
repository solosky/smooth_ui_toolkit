#include "sut_spring.h"
#include "sut_math.h"

void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt) {
    sut_real_t displacement = *x - target;
    sut_real_t force = sut_fp_mul(p->stiffness, displacement);
    force = -(force + sut_fp_mul(p->damping, s->velocity));
    sut_real_t accel = sut_fp_div(force, p->mass);
    s->velocity += sut_fp_mul(accel, dt);
    *x += sut_fp_mul(s->velocity, dt);
}
