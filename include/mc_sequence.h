#ifndef MC_SEQUENCE_H
#define MC_SEQUENCE_H

#include <stdbool.h>
#include "mc_types.h"
#include "mc_easing.h"
#include "mc_errors.h"
#include "mc_allocator.h"

typedef struct {
    mc_real_t      target;
    mc_real_t      duration;
    mc_easing_fn_t easing;
} mc_animate_step_t;

typedef struct {
    mc_animate_step_t* steps;
    void (*on_step)(void* ctx, int index);
    void (*on_complete)(void* ctx);
    void*   ctx;
    uint16_t capacity;
    uint16_t count;
    uint16_t current;
    mc_real_t elapsed;
    mc_real_t from;
} mc_sequence_t;

int   mc_sequence_init(mc_sequence_t* s, uint16_t capacity);
void  mc_sequence_deinit(mc_sequence_t* s);
int   mc_sequence_push(mc_sequence_t* s, mc_animate_step_t step);
void  mc_sequence_clear(mc_sequence_t* s);
bool  mc_sequence_update(mc_sequence_t* s, mc_real_t dt);
void  mc_sequence_play(mc_sequence_t* s);

#endif
