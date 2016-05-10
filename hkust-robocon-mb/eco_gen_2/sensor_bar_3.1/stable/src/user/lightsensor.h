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

#define DELAY_US 600 //800
#define scalar 1000
#define MAXSAMPLETIMES 60

#define RIGHT

#ifdef LEFT
#define DARKGREENLEFT 180
#define DARKGREENRIGHT 220
#define DARKGREENBORDER 48
#define ORANGELEFT 15
#define ORANGERIGHT 38
#define ORANGEBORDER 60
#define RIVERLEFT 220
#define RIVERRIGHT 240
#define RIVERBORDER 51
#define LIGHTGREENLEFT 100
#define LIGHTGREENRIGHT 180
#define LIGHTGREENBORDER 14
#define PINKLEFT 38
#define PINKRIGHT 100
#define PINKBORDER 65
#endif

#ifdef RIGHT
#define DARKGREENLEFT 180
#define DARKGREENRIGHT 220
#define DARKGREENBORDER 49
#define ORANGELEFT 15
#define ORANGERIGHT 75
#define ORANGEBORDER 65
#define RIVERLEFT 220
#define RIVERRIGHT 235
#define RIVERBORDER 54
#define LIGHTGREENLEFT 100
#define LIGHTGREENRIGHT 180
#define LIGHTGREENBORDER 16
#define PINKLEFT 235
#define PINKRIGHT 270
#define PINKBORDER 10
#endif

typedef enum{
	RED = 0, GREEN = 1, BLUE = 2, OFF = 3
}Colour;

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
void LED_Control(u8 R, u8 G, u8 B);
void receive(CanRxMsg msg);

#endif