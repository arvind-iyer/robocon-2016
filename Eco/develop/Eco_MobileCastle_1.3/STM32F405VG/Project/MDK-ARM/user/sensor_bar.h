#ifndef _SENSORBAR_H
#define _SENSORBAR_H

#include "can_protocol.h"
#include "lcd_main.h"
#include <stdbool.h>
#include "servo_interface.h"

#define SENSOR_BAR_FILTER_1 0x0C5
#define SENSOR_BAR_FILTER_2 0x0C6
#define SENSOR_BAR_FILTER_3 0x0C7
#define SENSOR_BAR_FILTER_4 0x0C8

#define WHITE_LINE_WIDTH 3
#define SENSOR_BAR_MID 8
#define ALL_WHITE_LENGTH 10

typedef enum{
	SENSOR_BAR_NORM = 0, //Normal
	SENSOR_BAR_NTH = 1, //Nothing is sensed
	SENSOR_BAR_EXT = 2, //Extreme condition
	SENSOR_BAR_ALL = 3 //Read all or mostly white
}SENSOR_BAR_FLAG;

typedef enum{
	RED_START = 0,
	BLUE_START = 1, 
	SLOPE_GREEN = 2,
	HIGH_ORANGE = 3,
	RIVER_BLUE = 4, 
	DOWN_GREEN = 5
}COLOR_REGIONS;

void sensorbar_init(void);
u16 sb_pwm_1to1(u16 inc_pwm, u16 dec_pwm);
//Use which shape for the correction, like error^2, error^3
SENSOR_BAR_FLAG sensor_bar_track(u8 power, u16 sensor_bar_Kp);
s16 sensor_bar_get_corr(u8 power, u16 sensor_bar_Kp, SENSOR_BAR_FLAG* in_flag);
s16 sensor_bar_get_corr_nf(u8 power, u16 sensor_bar_Kp);

extern u8 sensor_bar_mid;
extern u16 sensor_bar_filtered[16];
extern u8 sensorbar_region;
extern u8 sensorbar_cali;

#endif
