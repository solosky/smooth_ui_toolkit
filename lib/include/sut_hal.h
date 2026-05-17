#ifndef SUT_HAL_H
#define SUT_HAL_H

#include <stdint.h>

typedef uint32_t (*sut_get_tick_t)(void);
typedef void     (*sut_delay_t)(uint32_t ms);

void     sut_hal_set_tick_callback(sut_get_tick_t cb);
void     sut_hal_set_delay_callback(sut_delay_t cb);
uint32_t sut_get_tick(void);
void     sut_delay(uint32_t ms);

#endif
