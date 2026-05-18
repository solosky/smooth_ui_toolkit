#ifndef SUT_SIGNAL_H
#define SUT_SIGNAL_H

#include <stdint.h>
#include "sut_errors.h"

typedef void (*sut_slot_fn)(void* ctx, void* args);

typedef struct {
    sut_slot_fn callback;
    void*       ctx;
} sut_slot_t;

typedef struct {
    sut_slot_t* slots;
    uint16_t    capacity;
    uint16_t    count;
} sut_signal_t;

void sut_signal_init(sut_signal_t* sig, uint16_t capacity);
void sut_signal_deinit(sut_signal_t* sig);
int  sut_signal_connect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_disconnect(sut_signal_t* sig, sut_slot_fn cb, void* ctx);
void sut_signal_emit(sut_signal_t* sig, void* args);

#endif
