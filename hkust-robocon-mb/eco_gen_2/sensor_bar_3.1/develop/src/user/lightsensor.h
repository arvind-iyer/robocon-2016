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
#include "approx_math.h"
#include <stdlib.h>

#define DELAY_US 1000 //800
#define scalar 1000
#define MAXSAMPLETIMES 60
#define NUMOFAREAS 5
#define SATOFFSET 32

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
#define DARKGREENLEFT 160
#define DARKGREENRIGHT 200
#define DARKGREENBORDER 55
#define ORANGELEFT 15
#define ORANGERIGHT 75
#define ORANGEBORDER 69
#define RIVERLEFT 210
#define RIVERRIGHT 225
#define RIVERBORDER 45
#define LIGHTGREENLEFT 80
#define LIGHTGREENRIGHT 155
#define LIGHTGREENBORDER 16
#define PINKLEFT 226
#define PINKRIGHT 270
#define PINKBORDER 80
#endif

typedef enum{
	STARTOREND = 0, 
    DARKGREEN = 1, 
    ORANGE = 2, 
    RIVER = 3,
    LIGHTGREEN = 4,
    UNKNOWN = 5
}Colour;

typedef struct
{
	s32 red_reading[16];
	s32 blue_reading[16];
	s32 green_reading[16];
	s32 off_reading[16];
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
void analysisData(void);
void receive(CanRxMsg msg);

#endif
