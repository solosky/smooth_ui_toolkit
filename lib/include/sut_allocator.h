#ifndef SUT_ALLOCATOR_H
#define SUT_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    void* (*alloc)(size_t size, void* ctx);
    void  (*free)(void* ptr, void* ctx);
    void* ctx;
} sut_allocator_t;

void sut_allocator_set(sut_allocator_t* alloc);

void* sut_malloc(size_t size);
void  sut_free(void* ptr);

#endif
