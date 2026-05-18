#ifndef MC_SIGNAL_H
#define MC_SIGNAL_H

#include <stdint.h>
#include "mc_errors.h"

typedef void (*mc_slot_fn)(void* ctx, void* args);

typedef struct {
    mc_slot_fn callback;
    void*       ctx;
} mc_slot_t;

typedef struct {
    mc_slot_t* slots;
    uint16_t    capacity;
    uint16_t    count;
} mc_signal_t;

void mc_signal_init(mc_signal_t* sig, uint16_t capacity);
void mc_signal_deinit(mc_signal_t* sig);
int  mc_signal_connect(mc_signal_t* sig, mc_slot_fn cb, void* ctx);
void mc_signal_disconnect(mc_signal_t* sig, mc_slot_fn cb, void* ctx);
void mc_signal_emit(mc_signal_t* sig, void* args);

#endif
