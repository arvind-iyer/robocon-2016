#ifndef _ADC_IR_H
#define _ADC_IR_H

#include "stm32f4xx.h"
#include <math.h>
#include <stdbool.h>
#include "adc.h"

#define ADC_IR_MAX 		0.0033333333f //In mm, inverted
#define ADC_IR_MIN		0.0250000000f //In mm, inverted
#define ADC_IR_1_MIN	520
#define ADC_IR_1_MAX	3680
#define ADC_IR_2_MIN	520
#define ADC_IR_2_MAX	3680

s16 get_ir_dis(u8 i);

#endif
