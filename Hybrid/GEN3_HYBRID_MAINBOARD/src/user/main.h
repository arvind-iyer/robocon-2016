#ifndef __MAIN_H
#define __MAIN_H

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 
  
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_crc.h"

/*** Essential ***/
#include "ticks.h"
#include "delay.h"

#include "buzzer.h"
#include "led.h"
#include "lcd_main.h"
#include "system.h"
#include "xbc_mb.h"
#include "can_xbc_mb.h"
#include "button.h"
#include "encoder.h"

/*** Optional ***/
#include "buzzer_song.h"
#include "can.h"
#include "can_protocol.h"
#include "can_motor.h"
#include "uart.h"
#include "approx_math.h"
#include "servo.h"
#include "gyro.h"
#include "bluetooth.h"
#include "button.h"
#include "led.h"
#include "linear_ccd.h"
#include "adc.h"
#include "manual_mode.h"
#include "pneumatic.h"
#include "auto_mode.h"
#include "xbc_control.h"
#include "robot_control.h"
#include "pca9685.h"
#include "laser_sensor.h"
#include "RPM_sensor.h"

#define BLUE_FIELD
//#define RED_FIELD

#ifdef RED_FIELD
	#ifdef BLUE_FIELD
		#error FIELD COLOR BOTH DEFINED
	#endif
#else
	#ifndef BLUE_FIELD
		#error FIELD COLOR NOT YET DEFINED
	#endif
#endif

#ifdef BLUE_FIELD
	#define THIS_GRIPPER GRIPPER_1
#else
	#define THIS_GRIPPER GRIPPER_2
#endif

/**
** The following ticks is to avoid ticks passed in execution in order to give consistent result
**
** Short loop and long loop are bigger loops than the main loop, with a greater interval.
** The actual interval may be larger than the stated one, but will never be smaller.
** They are the actual ticks count instead of passed ticks(might be a possible improvement)
** They are updated in main.c
**/

//Main loop
extern u32 this_loop_ticks;
extern u32 last_loop_ticks;

//Larger loop
#define LONG_LOOP_TICKS 20
#define SHORT_LOOP_TICKS 1
extern u32 last_long_loop_ticks;
extern u32 last_short_loop_ticks;

#endif /* __MAIN_H */
