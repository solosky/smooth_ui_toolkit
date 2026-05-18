#ifndef SUT_ANIMATE_H
#define SUT_ANIMATE_H

#include <stdbool.h>
#include <string.h>
#include "sut_types.h"
#include "sut_math.h"
#include "sut_easing.h"
#include "sut_spring.h"

typedef enum {
    SUT_ANIMATE_EASING,
    SUT_ANIMATE_SPRING
} sut_animate_mode_t;

typedef struct {
    sut_real_t from;
    sut_real_t to;
    sut_real_t current;
    sut_real_t elapsed;
    sut_real_t delay;
    sut_real_t velocity;

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void    (*on_update)(void* ctx, sut_real_t value);
    void    (*on_complete)(void* ctx);
    void*   ctx;

    uint8_t mode;
    uint8_t repeat;
    uint8_t repeat_count;
} sut_animate_t;

void sut_animate_init_easing(sut_animate_t* a, sut_real_t from, sut_real_t to, sut_real_t duration);
void sut_animate_init_spring(sut_animate_t* a, sut_real_t from, sut_real_t to);
bool sut_animate_update(sut_animate_t* a, sut_real_t dt);
void sut_animate_reset(sut_animate_t* a);

typedef struct {
    sut_vec2_t from;
    sut_vec2_t to;
    sut_vec2_t current;
    sut_real_t elapsed;
    sut_real_t delay;
    sut_vec2_t velocity;

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, sut_vec2_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} sut_vec2_animate_t;

void sut_vec2_animate_init_easing(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to, sut_real_t duration);
void sut_vec2_animate_init_spring(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to);
void sut_vec2_animate_move_to(sut_vec2_animate_t* a, sut_vec2_t target);
bool sut_vec2_animate_update(sut_vec2_animate_t* a, sut_real_t dt);

typedef struct {
    sut_vec4_t from;
    sut_vec4_t to;
    sut_vec4_t current;
    sut_real_t elapsed;
    sut_real_t delay;
    sut_vec4_t velocity;

    union {
        struct {
            sut_real_t       duration;
            sut_easing_fn_t  easing;
        } easing;
        sut_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, sut_vec4_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} sut_vec4_animate_t;

void sut_vec4_animate_init_easing(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to, sut_real_t duration);
void sut_vec4_animate_init_spring(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to);
void sut_vec4_animate_move_to(sut_vec4_animate_t* a, sut_vec4_t target);
bool sut_vec4_animate_update(sut_vec4_animate_t* a, sut_real_t dt);

#endif
