#ifndef _SENSORBAR_H
#define _SENSORBAR_H

#include "can_protocol.h"
#include "lcd_main.h"
#include <stdbool.h>
#include "servo_interface.h"

#define SENSOR_BAR_FILTER_1 0x0C5
#define SENSOR_BAR_FILTER_2 0x0C6

#define WHITE_LINE_WIDTH 3
#define SENSOR_BAR_MID 8

//Scaled by 100
#define SERVO_SENSOR_MAX 140
#define SERVO_SENSOR_MIN 40

typedef enum{
	SENSOR_BAR_NORM = 0, //Normal
	SENSOR_BAR_NTH = 1, //Nothing is sensed
	SENSOR_BAR_EXT = 2 //Extreme condition
}SENSOR_BAR_FLAG;

void sensorbar_init(void);

//Use which shape for the correction, like error^2, error^3
SENSOR_BAR_FLAG sensor_bar_track(u8 power, u16 sensor_bar_Kp);
s16 sensor_bar_get_corr(u8 power, u16 sensor_bar_Kp, SENSOR_BAR_FLAG* in_flag);
s16 sensor_bar_get_corr_nf(u8 power, u16 sensor_bar_Kp);

extern u16 sensorbar_value[16];

#endif
