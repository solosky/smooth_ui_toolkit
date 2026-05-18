#include "mc_pool.h"
#include "mc_allocator.h"
#include <string.h>
#include <stdint.h>

int mc_pool_init(mc_pool_t* pool, uint16_t object_size, uint16_t capacity) {
    pool->memory = (uint8_t*)mc_malloc((size_t)object_size * capacity);
    pool->free_list = (int*)mc_malloc((size_t)capacity * sizeof(int));
    pool->used_flags = (uint8_t*)mc_malloc(capacity);
    if (!pool->memory || !pool->free_list || !pool->used_flags) {
        mc_free(pool->memory); mc_free(pool->free_list); mc_free(pool->used_flags);
        return MC_ERR_NOMEM;
    }
    pool->object_size = object_size;
    pool->capacity = capacity;
    pool->free_count = (int16_t)capacity;
    for (uint16_t i = 0; i < capacity; i++) {
        pool->free_list[i] = (int)i;
        pool->used_flags[i] = 0;
    }
    return MC_OK;
}

void mc_pool_deinit(mc_pool_t* pool) {
    mc_free(pool->memory);
    mc_free(pool->free_list);
    mc_free(pool->used_flags);
    memset(pool, 0, sizeof(*pool));
}

void* mc_pool_acquire(mc_pool_t* pool, mc_pool_ctor_fn ctor, void* ctx) {
    if (pool->free_count <= 0) return NULL;
    int index = pool->free_list[pool->free_count - 1];
    pool->free_count--;
    pool->used_flags[index] = 1;
    void* obj = pool->memory + (size_t)index * pool->object_size;
    if (ctor) ctor(obj, ctx);
    return obj;
}

void mc_pool_release(mc_pool_t* pool, void* obj) {
    uintptr_t offset = (uintptr_t)((uint8_t*)obj - pool->memory);
    if (offset % pool->object_size != 0) return;
    int index = (int)(offset / pool->object_size);
    if (index < 0 || (uint16_t)index >= pool->capacity || !pool->used_flags[index]) return;
    pool->used_flags[index] = 0;
    pool->free_list[pool->free_count] = index;
    pool->free_count++;
}

int mc_pool_available(mc_pool_t* pool) {
    return pool->free_count;
}

void mc_pool_foreach(mc_pool_t* pool, void (*fn)(void* obj, void* ctx), void* ctx) {
    for (uint16_t i = 0; i < pool->capacity; i++) {
        if (pool->used_flags[i])
            fn(pool->memory + (size_t)i * pool->object_size, ctx);
    }
}
