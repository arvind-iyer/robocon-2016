#ifndef _ADC_IR_H
#define _ADC_IR_H

#include "stm32f10x.h"
#include <math.h>
#include <stdbool.h>
#include "adc.h"

#define ADC_BUFFER_SIZE 10
#define ADC_IR_MAX 		333 //In mm, inverted //Scaled by 100000
#define ADC_IR_MIN		2500 //In mm, inverted //Scaled by 100000
#define ADC_IR_1_MIN	520
#define ADC_IR_1_MAX	3680
#define ADC_IR_CHANNEL 3

void ir_update(void);
s16 get_ir_dis(void);

#endif
