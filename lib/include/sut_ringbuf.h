#ifndef SUT_RINGBUF_H
#define SUT_RINGBUF_H

#include <stdint.h>
#include <stdbool.h>
#include "sut_errors.h"

typedef struct {
    uint8_t*  buffer;
    uint16_t  element_size;
    uint16_t  capacity;
    uint16_t  head;
    uint16_t  tail;
    uint16_t  count;
    uint8_t   overwrite;
} sut_ringbuf_t;

int  sut_ringbuf_init(sut_ringbuf_t* rb, uint16_t element_size, uint16_t capacity);
void sut_ringbuf_deinit(sut_ringbuf_t* rb);
int  sut_ringbuf_push(sut_ringbuf_t* rb, const void* data);
int  sut_ringbuf_pop(sut_ringbuf_t* rb, void* out);
int  sut_ringbuf_peek(sut_ringbuf_t* rb, uint16_t index, void* out);
void sut_ringbuf_clear(sut_ringbuf_t* rb);
int  sut_ringbuf_count(sut_ringbuf_t* rb);

#endif
