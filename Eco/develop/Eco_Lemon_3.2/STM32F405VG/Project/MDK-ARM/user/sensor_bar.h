#ifndef _SENSORBAR_H
#define _SENSORBAR_H

#include "can_protocol.h"
#include "lcd_main.h"
#include <stdbool.h>
#include "usart.h"
#include "servo_interface.h"
#include "main.h"

#define SENSOR_BAR_FILTER_1 0x0C5
#define SENSOR_BAR_FILTER_2 0x0C6
#define SENSOR_BAR_FILTER_3 0x0C7
#define SENSOR_BAR_FILTER_4 0x0C8

#define TOTAL_COLOR			6
#define CALI_PRE_COLOR	3
#define REGIONS (TOTAL_COLOR*CALI_PRE_COLOR)
#define REGION_BUFFER_SIZE 10

#define WHITE_LINE_WIDTH 3
#define SENSOR_BAR_MID 8
#define ALL_WHITE_LENGTH 12

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
}COLOR_REGION;

void sensorbar_init(void);
void request_color_msg(void);
u16 sb_pwm_1to1(u16 inc_pwm, u16 dec_pwm, s8 sensor_bar_bias);
//Use which shape for the correction, like error^2, error^3
SENSOR_BAR_FLAG sensor_bar_track(u8 power, u16 sensor_bar_Kp);
s16 sensor_bar_get_corr(u8 power, u16 sensor_bar_Kp, SENSOR_BAR_FLAG* in_flag);

extern u8 sensor_bar_mid;
extern u16 sensor_bar_filtered[16];
extern u8 sensorbar_region;
extern u8 sensorbar_cali;
extern s16 compensated_region_color[REGIONS][3];
extern s16 region_color_average[REGIONS][3];
extern u16 sensorbar_receive_diff;

#endif
