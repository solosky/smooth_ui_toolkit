#include "mc_hal.h"
#include <stddef.h>

static mc_get_tick_t _get_tick = NULL;
static mc_delay_t _delay = NULL;

void mc_hal_set_tick_callback(mc_get_tick_t cb) { _get_tick = cb; }
void mc_hal_set_delay_callback(mc_delay_t cb) { _delay = cb; }

uint32_t mc_get_tick(void) {
    return _get_tick ? _get_tick() : 0;
}

void mc_delay(uint32_t ms) {
    if (_delay) _delay(ms);
}
