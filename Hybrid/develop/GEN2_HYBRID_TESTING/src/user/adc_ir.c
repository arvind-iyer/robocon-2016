#include "adc_ir.h"

//Moving average
static s16 ir_buffer[ADC_BUFFER_SIZE] = {0};
static u8 ir_buffer_pointer = 0;

void ir_update(){
	ir_buffer[ir_buffer_pointer] = (ADC_IR_1_MAX - ADC_IR_1_MIN)*10000/(get_adc(ADC_IR_CHANNEL)*(ADC_IR_MIN - ADC_IR_MAX));
	ir_buffer_pointer = (ir_buffer_pointer+1)%ADC_BUFFER_SIZE;
}

s16 get_ir_dis(){
	s32 sum = 0;
	for (u8 i=0;i<ADC_BUFFER_SIZE;i++){
		sum += ir_buffer[i];
	}
	return (sum/ADC_BUFFER_SIZE);
}
