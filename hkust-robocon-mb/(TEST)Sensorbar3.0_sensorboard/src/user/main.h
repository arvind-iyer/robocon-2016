#ifndef __MAIN_H
#define __MAIN_H

//#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
//#define BYTETOBINARY(byte)  \
//  (byte & 0x80 ? 1 : 0), \
//  (byte & 0x40 ? 1 : 0), \
//  (byte & 0x20 ? 1 : 0), \
//  (byte & 0x10 ? 1 : 0), \
//  (byte & 0x08 ? 1 : 0), \
//  (byte & 0x04 ? 1 : 0), \
//  (byte & 0x02 ? 1 : 0), \
//  (byte & 0x01 ? 1 : 0) 

#include "stm32f10x.h"
#include "uart.h"
//#include "can.h"
//#include "can_protocol.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "ticks.h"
#include "delay.h"
#include "led.h"
#include "colour_calculation.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

#endif
