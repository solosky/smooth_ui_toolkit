#include "mc_sequence.h"
#include "mc_math.h"
#include <string.h>
#include "mc_errors.h"
#include "mc_allocator.h"

int mc_sequence_init(mc_sequence_t* s, uint16_t capacity) {
    memset(s, 0, sizeof(*s));
    s->steps = (mc_animate_step_t*)mc_malloc(capacity * sizeof(mc_animate_step_t));
    if (!s->steps) return MC_ERR_NOMEM;
    s->capacity = capacity;
    return MC_OK;
}

void mc_sequence_deinit(mc_sequence_t* s) {
    mc_free(s->steps);
    memset(s, 0, sizeof(*s));
}

int mc_sequence_push(mc_sequence_t* s, mc_animate_step_t step) {
    if (s->count >= s->capacity) return MC_ERR_FULL;
    s->steps[s->count++] = step;
    return MC_OK;
}

void mc_sequence_clear(mc_sequence_t* s) {
    s->count = 0;
    s->current = 0;
    s->elapsed = 0;
}

void mc_sequence_play(mc_sequence_t* s) {
    s->current = 0;
    s->elapsed = 0;
    if (s->count > 0)
        s->from = 0;
    if (s->count > 0 && s->on_step)
        s->on_step(s->ctx, 0);
}

bool mc_sequence_update(mc_sequence_t* s, mc_real_t dt) {
    if (s->current >= s->count) return true;
    s->elapsed += dt;
    while (s->current < s->count) {
        mc_real_t t = s->elapsed;
        if (s->steps[s->current].duration > 0)
            t = mc_fp_div(s->elapsed, s->steps[s->current].duration);
        if (t > MC_FP_C(1)) {
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
