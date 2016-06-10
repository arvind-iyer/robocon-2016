#ifndef DUAL_CHEAP_LASER_H
#define DUAL_CHEAP_LASER_H

#include "stm32f10x.h"
#include "uart.h"
#include "ticks.h"
#include "lcd_main.h"
#include "main.h"

void dual_laser_init(void);
s16 get_cheap_laser_dis(u8 id);
void dual_laser_init_update(void);
bool is_dual_laser_ready(void);

#define DUAL_INIT_TICKS_DELAY 100
#define DUAL_LASER1_UART COM3
#define DUAL_LASER2_UART COM4

#endif
