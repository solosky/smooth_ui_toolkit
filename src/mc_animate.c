#include "mc_animate.h"
#include "mc_math.h"
#include <string.h>

void mc_animate_init_easing(mc_animate_t* a, mc_real_t from, mc_real_t to, mc_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = mc_ease_linear;
}

void mc_animate_init_spring(mc_animate_t* a, mc_real_t from, mc_real_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = MC_FP_C(100);
    a->config.spring.damping = MC_FP_C(10);
    a->config.spring.mass = MC_FP_C(1);
}

bool mc_animate_update(mc_animate_t* a, mc_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == MC_ANIMATE_SPRING) {
        mc_spring_step(&a->current, (mc_spring_state_t*)&a->velocity, &a->config.spring, a->to, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    mc_real_t t = mc_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= MC_FP_C(1)) {
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
    mc_real_t eased = a->config.easing.easing(t);
    a->current = mc_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

void mc_animate_reset(mc_animate_t* a) {
    a->elapsed = 0;
    a->delay = 0;
    a->velocity = 0;
    a->current = a->from;
    a->repeat_count = 0;
}

// ---------------------------------------------------------------------------
// vec2 animate
// ---------------------------------------------------------------------------

void mc_vec2_animate_init_easing(mc_vec2_animate_t* a, mc_vec2_t from, mc_vec2_t to, mc_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = mc_ease_linear;
}

void mc_vec2_animate_init_spring(mc_vec2_animate_t* a, mc_vec2_t from, mc_vec2_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = MC_FP_C(100);
    a->config.spring.damping = MC_FP_C(10);
    a->config.spring.mass = MC_FP_C(1);
}

void mc_vec2_animate_move_to(mc_vec2_animate_t* a, mc_vec2_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity.x = 0;
    a->velocity.y = 0;
}

bool mc_vec2_animate_update(mc_vec2_animate_t* a, mc_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == MC_ANIMATE_SPRING) {
        mc_spring_step(&a->current.x, (mc_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        mc_spring_step(&a->current.y, (mc_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    mc_real_t t = mc_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= MC_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    mc_real_t eased = a->config.easing.easing(t);
    a->current = mc_vec2_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}

// ---------------------------------------------------------------------------
// vec4 animate
// ---------------------------------------------------------------------------

void mc_vec4_animate_init_easing(mc_vec4_animate_t* a, mc_vec4_t from, mc_vec4_t to, mc_real_t duration) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_EASING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.easing.duration = duration;
    a->config.easing.easing = mc_ease_linear;
}

void mc_vec4_animate_init_spring(mc_vec4_animate_t* a, mc_vec4_t from, mc_vec4_t to) {
    memset(a, 0, sizeof(*a));
    a->mode = MC_ANIMATE_SPRING;
    a->from = from;
    a->to = to;
    a->current = from;
    a->config.spring.stiffness = MC_FP_C(100);
    a->config.spring.damping = MC_FP_C(10);
    a->config.spring.mass = MC_FP_C(1);
}

void mc_vec4_animate_move_to(mc_vec4_animate_t* a, mc_vec4_t target) {
    a->from = a->current;
    a->to = target;
    a->elapsed = 0;
    a->velocity.x = 0;
    a->velocity.y = 0;
    a->velocity.z = 0;
    a->velocity.w = 0;
}

bool mc_vec4_animate_update(mc_vec4_animate_t* a, mc_real_t dt) {
    if (a->delay > 0) {
        a->delay -= dt;
        if (a->delay > 0) return false;
        dt = -a->delay;
        a->delay = 0;
    }

    if (a->mode == MC_ANIMATE_SPRING) {
        mc_spring_step(&a->current.x, (mc_spring_state_t*)&a->velocity.x, &a->config.spring, a->to.x, dt);
        mc_spring_step(&a->current.y, (mc_spring_state_t*)&a->velocity.y, &a->config.spring, a->to.y, dt);
        mc_spring_step(&a->current.z, (mc_spring_state_t*)&a->velocity.z, &a->config.spring, a->to.z, dt);
        mc_spring_step(&a->current.w, (mc_spring_state_t*)&a->velocity.w, &a->config.spring, a->to.w, dt);
        if (a->on_update) a->on_update(a->ctx, a->current);
        return false;
    }

    a->elapsed += dt;
    mc_real_t t = mc_fp_div(a->elapsed, a->config.easing.duration);
    if (t >= MC_FP_C(1)) {
        a->current = a->to;
        if (a->on_update) a->on_update(a->ctx, a->current);
        if (a->on_complete) a->on_complete(a->ctx);
        return true;
    }
    mc_real_t eased = a->config.easing.easing(t);
    a->current = mc_vec4_lerp(a->from, a->to, eased);
    if (a->on_update) a->on_update(a->ctx, a->current);
    return false;
}
