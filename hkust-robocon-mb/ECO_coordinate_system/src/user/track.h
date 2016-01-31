#ifndef _TRACK_H
#define _TRACK_H

#include "stm32f10x.h"
#include "encoder.h"
#include "servo.h"
#include "lcd_main.h"
#include "button.h"
#include "led.h"

#define track_segment_number 11

void track_init(void);
void track_update(void);
void track_test(void);

#endif