#ifndef MC_RINGBUF_H
#define MC_RINGBUF_H

#include <stdint.h>
#include <stdbool.h>
#include "mc_errors.h"

typedef struct {
    uint8_t*  buffer;
    uint16_t  element_size;
    uint16_t  capacity;
    uint16_t  head;
    uint16_t  tail;
    uint16_t  count;
    uint8_t   overwrite;
} mc_ringbuf_t;

int  mc_ringbuf_init(mc_ringbuf_t* rb, uint16_t element_size, uint16_t capacity);
void mc_ringbuf_deinit(mc_ringbuf_t* rb);
int  mc_ringbuf_push(mc_ringbuf_t* rb, const void* data);
int  mc_ringbuf_pop(mc_ringbuf_t* rb, void* out);
int  mc_ringbuf_peek(mc_ringbuf_t* rb, uint16_t index, void* out);
void mc_ringbuf_clear(mc_ringbuf_t* rb);
int  mc_ringbuf_count(mc_ringbuf_t* rb);

#endif
