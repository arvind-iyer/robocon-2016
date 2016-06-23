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
#include "main.h"

#define DELAY_US 700 //800
#define scalar 1000
#define SAMPELS_TIMES 40

#define TOTAL_COLOR			6
#define CALI_PRE_COLOR	3

#define REGIONS (TOTAL_COLOR*CALI_PRE_COLOR)

typedef enum{
	RED = 0, GREEN = 1, BLUE = 2, OFF = 3
}Colour;

typedef struct
{
	s16 red_reading[16];
	s16 blue_reading[16];
	s16 green_reading[16];
	s16 off_reading[16];
}Reading;

extern s16 reading_in_area[REGIONS][2][3];
extern s16 compensated_region_color[REGIONS][3];
extern s16 region_color_average[REGIONS][3];
extern s16 sensor_max[16];
extern Reading full_white_reading;

void sensor_cali(u8 cali_stage);
void dataCollect(void);
void init_all_zero(void);
void printInformation(void);
void sendData(void);
void LED_Control(u8 R, u8 G, u8 B);

#endif
