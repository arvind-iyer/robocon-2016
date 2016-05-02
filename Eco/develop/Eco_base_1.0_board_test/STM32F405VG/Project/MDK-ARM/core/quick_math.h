#ifndef _QUICK_MATH_H
#define _QUICK_MATH_H

#include "stm32f4xx.h"

s16 get_median_of_five(s16 a, s16 b, s16 c, s16 d, s16 e);

extern __inline int16_t s16_cap(int16_t in_num, int16_t upper_bound, int16_t lower_bound);
extern __inline uint8_t u16_sqrt(uint16_t in_num);
extern __inline uint16_t u32_sqrt(uint32_t in_num);

#endif