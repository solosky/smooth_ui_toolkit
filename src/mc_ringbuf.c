#include "mc_ringbuf.h"
#include "mc_allocator.h"
#include <string.h>

int mc_ringbuf_init(mc_ringbuf_t* rb, uint16_t element_size, uint16_t capacity) {
    rb->buffer = (uint8_t*)mc_malloc((size_t)element_size * capacity);
    if (!rb->buffer) return MC_ERR_NOMEM;
    rb->element_size = element_size;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overwrite = 0;
    return MC_OK;
}

void mc_ringbuf_deinit(mc_ringbuf_t* rb) {
    mc_free(rb->buffer);
    memset(rb, 0, sizeof(*rb));
}

int mc_ringbuf_push(mc_ringbuf_t* rb, const void* data) {
    if (rb->count >= rb->capacity) {
        if (!rb->overwrite) return MC_ERR_FULL;
        rb->head = (rb->head + 1) % rb->capacity;
    } else {
        rb->count++;
    }
    memcpy(rb->buffer + rb->tail * rb->element_size, data, rb->element_size);
    rb->tail = (rb->tail + 1) % rb->capacity;
    return MC_OK;
}

int mc_ringbuf_pop(mc_ringbuf_t* rb, void* out) {
    if (rb->count == 0) return MC_ERR_EMPTY;
    memcpy(out, rb->buffer + rb->head * rb->element_size, rb->element_size);
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;
    return MC_OK;
}

int mc_ringbuf_peek(mc_ringbuf_t* rb, uint16_t index, void* out) {
    if (index >= rb->count) return MC_ERR_INVAL;
    uint16_t pos = (rb->head + index) % rb->capacity;
    memcpy(out, rb->buffer + pos * rb->element_size, rb->element_size);
    return MC_OK;
}

void mc_ringbuf_clear(mc_ringbuf_t* rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

int mc_ringbuf_count(mc_ringbuf_t* rb) {
    return (int)rb->count;
}
