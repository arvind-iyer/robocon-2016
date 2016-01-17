
#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include "stm32f10x.h"
#include "ticks.h"
#include "LEDs.h"
#include "tft_display.h"
#include "adc.h"
#include "linear_ccd.h"
#include "buzzer.h"
#include "buzzer_song.h"
#include "helper.h"
#include "motor.h"
#include "GPIO_switch.h"
#include "button.h"
#include "uart.h"
#include "servo_tutorial.h"
#include "motor_second.h"
#include "motor_first.h"
#include "pneumatic_control.h"
#include "approxmath.h"
#include "lineSensor.h"
#include "helper.h"

#define MAX_CCRn			1000
#define MIN_CCRn			450
#define GetCCRn(deg)	(((float)deg / 180) * 600 + MIN_CCRn)
u32 Pixel1AverageValue=64;
u32 Pixel2AverageValue=64;
u16 magnitude_second_motor=200;
#endif /* __MAIN_H */
