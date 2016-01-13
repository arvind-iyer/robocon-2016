#ifndef __LINEAR_CCD_H
#define __LINEAR_CCD_H

#include "adc.h"
#include "stm32f10x.h"
#include "ticks.h"
#include "lcd_main.h"

extern u32 linear_ccd_buffer[128];

#define SI_PORT GPIOB
#define SI_PIN GPIO_Pin_0

#define AO_PORT GPIOB
#define AO_PIN GPIO_Pin_1

#define CLK_PORT GPIOA
#define CLK_PIN GPIO_Pin_9

#define ADC_CHANNEL 2

void linear_ccd_init(void);
void linear_ccd_read(void);

#endif
