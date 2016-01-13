#ifndef __LINEAR_CCD_H
#define __LINEAR_CCD_H

#include "adc.h"
#include "stm32f10x.h"
#include "ticks.h"
#include "tft_display.h"

extern u16 linear_ccd_buffer1[128];
extern u16 linear_ccd_buffer2[128];

#define CLK_PORT GPIOA
#define CLK_PIN GPIO_Pin_8

#define SI1_PORT GPIOA
#define SI1_PIN GPIO_Pin_0

#define SI2_PORT GPIOB
#define SI2_PIN GPIO_Pin_0

#define AO1_channel 1
#define AO2_channel 2

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y)) 


void linear_ccd_init(void);
void linear_ccd_read(void);

#endif
