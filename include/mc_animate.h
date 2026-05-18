#ifndef MC_ANIMATE_H
#define MC_ANIMATE_H

#include <stdbool.h>
#include <string.h>
#include "mc_types.h"
#include "mc_math.h"
#include "mc_easing.h"
#include "mc_spring.h"

typedef enum {
    MC_ANIMATE_EASING,
    MC_ANIMATE_SPRING
} mc_animate_mode_t;

typedef struct {
    mc_real_t from;
    mc_real_t to;
    mc_real_t current;
    mc_real_t elapsed;
    mc_real_t delay;
    mc_real_t velocity;

    union {
        struct {
            mc_real_t       duration;
            mc_easing_fn_t  easing;
        } easing;
        mc_spring_params_t spring;
    } config;

    void    (*on_update)(void* ctx, mc_real_t value);
    void    (*on_complete)(void* ctx);
    void*   ctx;

    uint8_t mode;
    uint8_t repeat;
    uint8_t repeat_count;
} mc_animate_t;

void mc_animate_init_easing(mc_animate_t* a, mc_real_t from, mc_real_t to, mc_real_t duration);
void mc_animate_init_spring(mc_animate_t* a, mc_real_t from, mc_real_t to);
bool mc_animate_update(mc_animate_t* a, mc_real_t dt);
void mc_animate_reset(mc_animate_t* a);

typedef struct {
    mc_vec2_t from;
    mc_vec2_t to;
    mc_vec2_t current;
    mc_real_t elapsed;
    mc_real_t delay;
    mc_vec2_t velocity;

    union {
        struct {
            mc_real_t       duration;
            mc_easing_fn_t  easing;
        } easing;
        mc_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, mc_vec2_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} mc_vec2_animate_t;

void mc_vec2_animate_init_easing(mc_vec2_animate_t* a, mc_vec2_t from, mc_vec2_t to, mc_real_t duration);
void mc_vec2_animate_init_spring(mc_vec2_animate_t* a, mc_vec2_t from, mc_vec2_t to);
void mc_vec2_animate_move_to(mc_vec2_animate_t* a, mc_vec2_t target);
bool mc_vec2_animate_update(mc_vec2_animate_t* a, mc_real_t dt);

typedef struct {
    mc_vec4_t from;
    mc_vec4_t to;
    mc_vec4_t current;
    mc_real_t elapsed;
    mc_real_t delay;
    mc_vec4_t velocity;

    union {
        struct {
            mc_real_t       duration;
            mc_easing_fn_t  easing;
        } easing;
        mc_spring_params_t spring;
    } config;

    void (*on_update)(void* ctx, mc_vec4_t value);
    void (*on_complete)(void* ctx);
    void* ctx;

    uint8_t mode;
} mc_vec4_animate_t;

void mc_vec4_animate_init_easing(mc_vec4_animate_t* a, mc_vec4_t from, mc_vec4_t to, mc_real_t duration);
void mc_vec4_animate_init_spring(mc_vec4_animate_t* a, mc_vec4_t from, mc_vec4_t to);
void mc_vec4_animate_move_to(mc_vec4_animate_t* a, mc_vec4_t target);
bool mc_vec4_animate_update(mc_vec4_animate_t* a, mc_real_t dt);

#endif
