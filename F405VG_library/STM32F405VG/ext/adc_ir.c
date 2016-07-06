/**
* A library for GP2Y0A41SK ADC IR sensor
* Range: 4~30 cm, out of range reading is unreliable
*	Within range, voltage level has linear relationship with 1/distance
**/

#include "adc_ir.h"

s16 get_ir_dis(u8 i){
	switch(i){
		case 0:
			return (s16)roundf(1/(adc_get(i)*(ADC_IR_MIN - ADC_IR_MAX)/(ADC_IR_1_MAX - ADC_IR_1_MIN)));
		case 1:
			return (s16)roundf(1/(adc_get(i)*(ADC_IR_MIN - ADC_IR_MAX)/(ADC_IR_2_MAX - ADC_IR_2_MIN)));
		default:
			return 0;
	}
}
