#ifndef SUT_POOL_H
#define SUT_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include "sut_errors.h"

typedef void (*sut_pool_ctor_fn)(void* obj, void* ctx);
typedef void (*sut_pool_dtor_fn)(void* obj);

typedef struct {
    uint8_t*  memory;
    int*      free_list;
    uint8_t*  used_flags;
    uint16_t  object_size;
    uint16_t  capacity;
    int16_t   free_count;
} sut_pool_t;

int   sut_pool_init(sut_pool_t* pool, uint16_t object_size, uint16_t capacity);
void  sut_pool_deinit(sut_pool_t* pool);
void* sut_pool_acquire(sut_pool_t* pool, sut_pool_ctor_fn ctor, void* ctx);
void  sut_pool_release(sut_pool_t* pool, void* obj);
int   sut_pool_available(sut_pool_t* pool);

#endif
