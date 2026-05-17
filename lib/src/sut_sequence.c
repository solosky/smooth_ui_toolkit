#include "sut_sequence.h"
#include "sut_math.h"
#include <string.h>
#include "sut_errors.h"
#include "sut_allocator.h"

int sut_sequence_init(sut_sequence_t* s, uint16_t capacity) {
    memset(s, 0, sizeof(*s));
    s->steps = (sut_animate_step_t*)sut_malloc(capacity * sizeof(sut_animate_step_t));
    if (!s->steps) return SUT_ERR_NOMEM;
    s->capacity = capacity;
    return SUT_OK;
}

void sut_sequence_deinit(sut_sequence_t* s) {
    sut_free(s->steps);
    memset(s, 0, sizeof(*s));
}

int sut_sequence_push(sut_sequence_t* s, sut_animate_step_t step) {
    if (s->count >= s->capacity) return SUT_ERR_FULL;
    s->steps[s->count++] = step;
    return SUT_OK;
}

void sut_sequence_clear(sut_sequence_t* s) {
    s->count = 0;
    s->current = 0;
    s->elapsed = 0;
}

void sut_sequence_play(sut_sequence_t* s) {
    s->current = 0;
    s->elapsed = 0;
    if (s->count > 0)
        s->from = 0;
    if (s->count > 0 && s->on_step)
        s->on_step(s->ctx, 0);
}

bool sut_sequence_update(sut_sequence_t* s, sut_real_t dt) {
    if (s->current >= s->count) return true;
    s->elapsed += dt;
    while (s->current < s->count) {
        sut_real_t t = s->elapsed;
        if (s->steps[s->current].duration > 0)
            t = sut_fp_div(s->elapsed, s->steps[s->current].duration);
        if (t > SUT_FP_C(1)) {
            s->elapsed -= s->steps[s->current].duration;
            s->current++;
            if (s->current >= s->count) {
                if (s->on_complete) s->on_complete(s->ctx);
                return true;
            }
            if (s->on_step) s->on_step(s->ctx, s->current);
        } else {
            return false;
        }
    }
    return true;
}
