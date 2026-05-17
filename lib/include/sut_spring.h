#ifndef SUT_SPRING_H
#define SUT_SPRING_H

#include "sut_types.h"

typedef struct {
    sut_real_t stiffness;
    sut_real_t damping;
    sut_real_t mass;
} sut_spring_params_t;

typedef struct {
    sut_real_t velocity;
} sut_spring_state_t;

void sut_spring_step(sut_real_t* x, sut_spring_state_t* s, sut_spring_params_t* p, sut_real_t target, sut_real_t dt);

#endif
