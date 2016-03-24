#ifndef _SENSORBAR_H
#define _SENSORBAR_H

#include "can_protocol.h"
#include "lcd_main.h"
#include <stdbool.h>
#include "servo_targeting.h"

#define SENSOR_BAR_FILTER_1 0x0C5
#define SENSOR_BAR_FILTER_2 0x0C6

#define WHITE_LINE_WIDTH 3
#define SENSOR_BAR_MID 8

//Scaled by 100
#define SENSOR_BAR_MULT 350
#define SERVO_SENSOR_MAX 140
#define SERVO_SENSOR_MIN 40

void sensorbar_init(void);

//Use which shape for the correction, like error^2, error^3
void sensor_bar_track(u8 power);
s16 sensor_bar_get_corr(u8 power);

extern u16 sensorbar_value[16];

#endif
