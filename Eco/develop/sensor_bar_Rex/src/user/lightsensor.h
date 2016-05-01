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

#define DELAY_US 1200 //800
#define scalar 1000
#define SAMPELS_TIMES 60

typedef enum{
	RED = 0, GREEN = 1, BLUE = 2, OFF = 3
}Colour;

typedef struct
{
	u16 red_reading[16];
	u16 blue_reading[16];
	u16 green_reading[16];
	u16 off_reading[16];
}Reading;

void sensor_init(u8 cali_stage);
void initToZero(void);
void dataCollect(void);
void printInformation(void);
void sendData(void);
void LED_Control(u8 R, u8 G, u8 B);

#endif