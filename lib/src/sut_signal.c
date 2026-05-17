#include "sut_signal.h"
#include "sut_allocator.h"
#include <string.h>

void sut_signal_init(sut_signal_t* sig, uint16_t capacity) {
    sig->slots = (sut_slot_t*)sut_malloc(capacity * sizeof(sut_slot_t));
    sig->capacity = sig->slots ? capacity : 0;
    sig->count = 0;
}

void sut_signal_deinit(sut_signal_t* sig) {
    sut_free(sig->slots);
    memset(sig, 0, sizeof(*sig));
}

int sut_signal_connect(sut_signal_t* sig, sut_slot_fn cb, void* ctx) {
    if (sig->count >= sig->capacity) return SUT_ERR_FULL;
    sig->slots[sig->count].callback = cb;
    sig->slots[sig->count].ctx = ctx;
    sig->count++;
    return SUT_OK;
}

void sut_signal_disconnect(sut_signal_t* sig, sut_slot_fn cb, void* ctx) {
    for (uint16_t i = 0; i < sig->count; i++) {
        if (sig->slots[i].callback == cb && sig->slots[i].ctx == ctx) {
            sig->slots[i] = sig->slots[sig->count - 1];
            sig->count--;
            return;
        }
    }
}

void sut_signal_emit(sut_signal_t* sig, void* args) {
    for (uint16_t i = 0; i < sig->count; i++) {
        sig->slots[i].callback(sig->slots[i].ctx, args);
    }
}
