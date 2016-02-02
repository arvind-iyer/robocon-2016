#ifndef _MAIN_H
#define _MAIN_H

#include "stm32f10x.h"
#include "usart1.h"
#include "can.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "ticks.h"
#include "delay.h"
#include "flashsave.h"
#include "lightsensor.h"
#include "led.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

#define COLOUR_SWITCH_INTERVAL 10 //20

void CaptureData(void);
void DMA_init(void);
#endif
