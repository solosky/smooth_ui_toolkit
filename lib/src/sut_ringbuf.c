#include "sut_ringbuf.h"
#include "sut_allocator.h"
#include <string.h>

int sut_ringbuf_init(sut_ringbuf_t* rb, uint16_t element_size, uint16_t capacity) {
    rb->buffer = (uint8_t*)sut_malloc((size_t)element_size * capacity);
    if (!rb->buffer) return SUT_ERR_NOMEM;
    rb->element_size = element_size;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overwrite = 0;
    return SUT_OK;
}

void sut_ringbuf_deinit(sut_ringbuf_t* rb) {
    sut_free(rb->buffer);
    memset(rb, 0, sizeof(*rb));
}

int sut_ringbuf_push(sut_ringbuf_t* rb, const void* data) {
    if (rb->count >= rb->capacity) {
        if (!rb->overwrite) return SUT_ERR_FULL;
        rb->head = (rb->head + 1) % rb->capacity;
    } else {
        rb->count++;
    }
    memcpy(rb->buffer + rb->tail * rb->element_size, data, rb->element_size);
    rb->tail = (rb->tail + 1) % rb->capacity;
    return SUT_OK;
}

int sut_ringbuf_pop(sut_ringbuf_t* rb, void* out) {
    if (rb->count == 0) return SUT_ERR_EMPTY;
    memcpy(out, rb->buffer + rb->head * rb->element_size, rb->element_size);
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;
    return SUT_OK;
}

int sut_ringbuf_peek(sut_ringbuf_t* rb, uint16_t index, void* out) {
    if (index >= rb->count) return SUT_ERR_INVAL;
    uint16_t pos = (rb->head + index) % rb->capacity;
    memcpy(out, rb->buffer + pos * rb->element_size, rb->element_size);
    return SUT_OK;
}

void sut_ringbuf_clear(sut_ringbuf_t* rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

int sut_ringbuf_count(sut_ringbuf_t* rb) {
    return (int)rb->count;
}
