#ifndef _TRACK_H
#define _TRACK_H

#include "stm32f10x.h"
#include "encoder.h"
#include "servo.h"
#include "lcd_main.h"
#include "button.h"
#include "led.h"
#include "uart.h"
#include "line_sensor.h"

#define TRACK_SEGMENT_COUNT 5
#define TRACK_TOLERANCE 200
#define INCOMING_CCD_COM_PORT COM2

void track_init(void);
void track_update(void);
void track_test(void);

#endif