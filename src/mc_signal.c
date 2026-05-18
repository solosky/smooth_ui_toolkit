#include "mc_signal.h"
#include "mc_allocator.h"
#include <string.h>

void mc_signal_init(mc_signal_t* sig, uint16_t capacity) {
    sig->slots = (mc_slot_t*)mc_malloc(capacity * sizeof(mc_slot_t));
    sig->capacity = sig->slots ? capacity : 0;
    sig->count = 0;
}

void mc_signal_deinit(mc_signal_t* sig) {
    mc_free(sig->slots);
    memset(sig, 0, sizeof(*sig));
}

int mc_signal_connect(mc_signal_t* sig, mc_slot_fn cb, void* ctx) {
    if (sig->count >= sig->capacity) return MC_ERR_FULL;
    sig->slots[sig->count].callback = cb;
    sig->slots[sig->count].ctx = ctx;
    sig->count++;
    return MC_OK;
}

void mc_signal_disconnect(mc_signal_t* sig, mc_slot_fn cb, void* ctx) {
    for (uint16_t i = 0; i < sig->count; i++) {
        if (sig->slots[i].callback == cb && sig->slots[i].ctx == ctx) {
            sig->slots[i] = sig->slots[sig->count - 1];
            sig->count--;
            return;
        }
    }
}

void mc_signal_emit(mc_signal_t* sig, void* args) {
    for (uint16_t i = 0; i < sig->count; i++) {
        sig->slots[i].callback(sig->slots[i].ctx, args);
    }
}
