#include "sut_pool.h"
#include "sut_allocator.h"
#include <string.h>
#include <stdint.h>

int sut_pool_init(sut_pool_t* pool, uint16_t object_size, uint16_t capacity) {
    pool->memory = (uint8_t*)sut_malloc((size_t)object_size * capacity);
    pool->free_list = (int*)sut_malloc((size_t)capacity * sizeof(int));
    pool->used_flags = (uint8_t*)sut_malloc(capacity);
    if (!pool->memory || !pool->free_list || !pool->used_flags) {
        sut_free(pool->memory); sut_free(pool->free_list); sut_free(pool->used_flags);
        return SUT_ERR_NOMEM;
    }
    pool->object_size = object_size;
    pool->capacity = capacity;
    pool->free_count = (int16_t)capacity;
    for (uint16_t i = 0; i < capacity; i++) {
        pool->free_list[i] = (int)i;
        pool->used_flags[i] = 0;
    }
    return SUT_OK;
}

void sut_pool_deinit(sut_pool_t* pool) {
    sut_free(pool->memory);
    sut_free(pool->free_list);
    sut_free(pool->used_flags);
    memset(pool, 0, sizeof(*pool));
}

void* sut_pool_acquire(sut_pool_t* pool, sut_pool_ctor_fn ctor, void* ctx) {
    if (pool->free_count <= 0) return NULL;
    int index = pool->free_list[pool->free_count - 1];
    pool->free_count--;
    pool->used_flags[index] = 1;
    void* obj = pool->memory + (size_t)index * pool->object_size;
    if (ctor) ctor(obj, ctx);
    return obj;
}

void sut_pool_release(sut_pool_t* pool, void* obj) {
    uintptr_t offset = (uintptr_t)((uint8_t*)obj - pool->memory);
    if (offset % pool->object_size != 0) return;
    int index = (int)(offset / pool->object_size);
    if (index < 0 || (uint16_t)index >= pool->capacity || !pool->used_flags[index]) return;
    pool->used_flags[index] = 0;
    pool->free_list[pool->free_count] = index;
    pool->free_count++;
}

int sut_pool_available(sut_pool_t* pool) {
    return pool->free_count;
}
