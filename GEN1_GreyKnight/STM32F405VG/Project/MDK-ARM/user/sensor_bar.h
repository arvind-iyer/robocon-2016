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
#define SENSOR_BAR_KP 1500
#define SENSOR_BAR_KD 7500

void sensorbar_init(void);
void sensor_bar_track(void);

extern u16 sensorbar_value[16];

#endif
