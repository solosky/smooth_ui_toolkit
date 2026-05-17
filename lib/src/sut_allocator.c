#include "sut_allocator.h"
#include <stdlib.h>

static void* default_alloc(size_t size, void* ctx) {
    (void)ctx;
    return malloc(size);
}

static void default_free(void* ptr, void* ctx) {
    (void)ctx;
    free(ptr);
}

static sut_allocator_t g_alloc = { default_alloc, default_free, NULL };

void sut_allocator_set(sut_allocator_t* alloc) {
    if (alloc)
        g_alloc = *alloc;
}

void* sut_malloc(size_t size) {
    return g_alloc.alloc ? g_alloc.alloc(size, g_alloc.ctx) : NULL;
}

void sut_free(void* ptr) {
    if (g_alloc.free)
        g_alloc.free(ptr, g_alloc.ctx);
}
