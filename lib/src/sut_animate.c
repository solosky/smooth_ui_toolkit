#include "sut_animate.h"
#include "sut_math.h"
#include <string.h>

void sut_animate_init_easing(sut_animate_t* a, sut_real_t from, sut_real_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_animate_init_spring(sut_animate_t* a, sut_real_t from, sut_real_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

bool sut_animate_update(sut_animate_t* a, sut_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_step(&a->current, (sut_spring_state_t*)&a->velocity, &a->config.spring, a->to, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    sut_real_t t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->repeat > 0) {
            if (a->repeat != 255) {
                a->repeat_count++;
                if (a->repeat_count >= a->repeat) {
                    if (a->on_complete) a->on_complete(a->ctx);
                    return true;
                }
            }
            a->elapsed = 0;
            return false;
        }
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

void sut_animate_reset(sut_animate_t* a) {
    a->elapsed = 0;
    a->delay = 0;
    a->velocity = 0;
    a->current = a->from;
    a->repeat_count = 0;
}

// ---------------------------------------------------------------------------
// vec2 animate
// ---------------------------------------------------------------------------

void sut_vec2_animate_init_easing(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_vec2_animate_init_spring(sut_vec2_animate_t* a, sut_vec2_t from, sut_vec2_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

void sut_vec2_animate_move_to(sut_vec2_animate_t* a, sut_vec2_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity.x = 0;
    a->velocity.y = 0;
}

bool sut_vec2_animate_update(sut_vec2_animate_t* a, sut_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_step(&a->current.x, (sut_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        sut_spring_step(&a->current.y, (sut_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    sut_real_t t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_vec2_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

// ---------------------------------------------------------------------------
// vec4 animate
// ---------------------------------------------------------------------------

void sut_vec4_animate_init_easing(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to, sut_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = sut_ease_linear;
}

void sut_vec4_animate_init_spring(sut_vec4_animate_t* a, sut_vec4_t from, sut_vec4_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = SUT_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = SUT_FP_C(100);
    a->config.spring.damping = SUT_FP_C(10);
    a->config.spring.mass = SUT_FP_C(1);
}

void sut_vec4_animate_move_to(sut_vec4_animate_t* a, sut_vec4_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity.x = 0;
    a->velocity.y = 0;
    a->velocity.z = 0;
    a->velocity.w = 0;
}

bool sut_vec4_animate_update(sut_vec4_animate_t* a, sut_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == SUT_ANIMATE_SPRING) {
        sut_spring_step(&a->current.x, (sut_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        sut_spring_step(&a->current.y, (sut_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        sut_spring_step(&a->current.z, (sut_spring_state_t*)&a->velocity.z, &a->config.spring, a->to.z, dt);
        sut_spring_step(&a->current.w, (sut_spring_state_t*)&a->velocity.w, &a->config.spring, a->to.w, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    sut_real_t t = sut_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= SUT_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    sut_real_t eased = a->config.easing.easing(t);
    a->current = sut_vec4_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}
