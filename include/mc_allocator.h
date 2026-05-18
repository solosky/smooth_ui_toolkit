#ifndef MC_ALLOCATOR_H
#define MC_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    void* (*alloc)(size_t size, void* ctx);
    void  (*free)(void* ptr, void* ctx);
    void* ctx;
} mc_allocator_t;

void mc_allocator_set(mc_allocator_t* alloc);

void* mc_malloc(size_t size);
void  mc_free(void* ptr);

#endif
