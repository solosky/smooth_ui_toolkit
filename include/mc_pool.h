#ifndef MC_POOL_H
#define MC_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include "mc_errors.h"

typedef void (*mc_pool_ctor_fn)(void* obj, void* ctx);
typedef void (*mc_pool_dtor_fn)(void* obj);

typedef struct {
    uint8_t*  memory;
    int*      free_list;
    uint8_t*  used_flags;
    uint16_t  object_size;
    uint16_t  capacity;
    int16_t   free_count;
} mc_pool_t;

int   mc_pool_init(mc_pool_t* pool, uint16_t object_size, uint16_t capacity);
void  mc_pool_deinit(mc_pool_t* pool);
void* mc_pool_acquire(mc_pool_t* pool, mc_pool_ctor_fn ctor, void* ctx);
void  mc_pool_release(mc_pool_t* pool, void* obj);
int   mc_pool_available(mc_pool_t* pool);
void  mc_pool_foreach(mc_pool_t* pool, void (*fn)(void* obj, void* ctx), void* ctx);

#endif
