#ifndef SUT_SEQUENCE_H
#define SUT_SEQUENCE_H

#include <stdbool.h>
#include "sut_types.h"
#include "sut_easing.h"
#include "sut_errors.h"
#include "sut_allocator.h"

typedef struct {
    sut_real_t      target;
    sut_real_t      duration;
    sut_easing_fn_t easing;
} sut_animate_step_t;

typedef struct {
    sut_animate_step_t* steps;
    void (*on_step)(void* ctx, int index);
    void (*on_complete)(void* ctx);
    void*   ctx;
    uint16_t capacity;
    uint16_t count;
    uint16_t current;
    sut_real_t elapsed;
    sut_real_t from;
} sut_sequence_t;

int   sut_sequence_init(sut_sequence_t* s, uint16_t capacity);
void  sut_sequence_deinit(sut_sequence_t* s);
int   sut_sequence_push(sut_sequence_t* s, sut_animate_step_t step);
void  sut_sequence_clear(sut_sequence_t* s);
bool  sut_sequence_update(sut_sequence_t* s, sut_real_t dt);
void  sut_sequence_play(sut_sequence_t* s);

#endif
