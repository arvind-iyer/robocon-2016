#ifndef LIGHTSENSORH
#define LIGHTSENSORH

#include "stm32f10x.h"
#include "can.h"
#include "can_protocol.h"
#include "delay.h"
#include "flashMem.h"
#include "ticks.h"
#include "uart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include <stdlib.h>

#define DELAY_US 1000 //800
#define scalar 1000
typedef struct
{
	u16 red_reading[16];
	u16 blue_reading[16];
	u16 green_reading[16];
	u16 off_reading[16];
	u16 h[16];
	u16 s[16];
	u16 v[16];
}Reading;

void sensor_init(Reading*  max);
void initToZero(void);
void dataCollect(void);
void rgb_hsv_converter(Reading* reading);
void printInformation(void);
void sendData(void);

#endif