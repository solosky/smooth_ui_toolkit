#ifndef MC_HAL_H
#define MC_HAL_H

#include <stdint.h>

typedef uint32_t (*mc_get_tick_t)(void);
typedef void     (*mc_delay_t)(uint32_t ms);

void     mc_hal_set_tick_callback(mc_get_tick_t cb);
void     mc_hal_set_delay_callback(mc_delay_t cb);
uint32_t mc_get_tick(void);
void     mc_delay(uint32_t ms);

#endif
