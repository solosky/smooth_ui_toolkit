#ifndef MC_SPRING_H
#define MC_SPRING_H

#include "mc_types.h"

typedef struct {
    mc_real_t stiffness;
    mc_real_t damping;
    mc_real_t mass;
} mc_spring_params_t;

typedef struct {
    mc_real_t velocity;
} mc_spring_state_t;

void mc_spring_step(mc_real_t* x, mc_spring_state_t* s, mc_spring_params_t* p, mc_real_t target, mc_real_t dt);

#endif
