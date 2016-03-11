#ifndef __LASER_SENSOR_H
#define __LASER_SENSOR_H

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"

#define ls_port GPIOC
#define ls_pin GPIO_Pin_3
#define ls_rcc RCC_APB2Periph_GPIOC

#define min_adc						16			
#define max_adc					 	3033		
#define min_dis 					200			
#define max_dis 					5000
#define avg_length				10			//the total num of avg, increasing the number will provide more accuracy but decrease in response rtime

void ls_init();
u32 get_ls_reading();
void ls_avg_init();
void ls_avg();

#endif