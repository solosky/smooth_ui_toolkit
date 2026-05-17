#include "sut_hal.h"

static sut_get_tick_t _get_tick = NULL;
static sut_delay_t _delay = NULL;

void sut_hal_set_tick_callback(sut_get_tick_t cb) { _get_tick = cb; }
void sut_hal_set_delay_callback(sut_delay_t cb) { _delay = cb; }

uint32_t sut_get_tick(void) {
    return _get_tick ? _get_tick() : 0;
}

void sut_delay(uint32_t ms) {
    if (_delay) _delay(ms);
}
