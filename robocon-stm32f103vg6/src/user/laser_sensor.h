#ifndef __LASER_SENSOR_H
#define __LASER_SENSOR_H

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"

//#define ls_port GPIOA
#define ls_pin GPIO#ifndef __LASER_SENSOR_H
#define __LASER_SENSOR_H

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"

#define ls_port 					GPIOC
#define ls_pin1 					GPIO_Pin_3
#define ls_pin2 					GPIO_Pin_1 	
#define ls_rcc 						RCC_APB2Periph_GPIOC
#define	ls_adc_address		((uint32_t)0x4001244C)

//#define min_adc						18 
//#define max_adc					 	3036		
//#define min_dis 					200			
//#define max_dis 					2500
#define avg_length				10			//the total num of avg, increasing the number will provide more accuracy but decrease in response rtime

#define ls_number					2				//number of sensors used


void ls_init(void);
u32 get_ls_cal_reading(u8 device);
u32 get_ls_adc_reading(u8 device);
void ls_avg(u8 device);

#endif
