#ifndef _BURSHLESS_COUNTER_H
#define _BURSHLESS_COUNTER_H

#include "stm32f10x.h"
#include "ticks.h"

#define BRUSHLESS_COUNTER_GPIO &PE11
#define BRUSHLESS_COUNTER_TIM TIM1
#define BRUSHLESS_COUNTER_TIM_REMAP GPIO_FullRemap_TIM1

void brushless_counter_init(void);
s32 get_brushless_counter(void);

#endif
