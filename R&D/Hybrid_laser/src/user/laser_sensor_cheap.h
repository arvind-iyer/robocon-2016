#ifndef LASER_SENSOR_CHEAP_H
#define LASER_SENSOR_CHEAP_H

#include "stm32f10x.h"
#include "uart.h"
#include "ticks.h"
#include "lcd_main.h"

extern u8 laser_byte_array[3][100];
extern u8 laser_byte_pointer[3];
extern u32 ticks_diff_laser[3];

void cheap_laser_init(void);

#define CHEAP_LASER_1_UART COM3
#define CHEAP_LASER_2_UART COM4
#define CHEAP_LASER_3_UART COM5

#endif
