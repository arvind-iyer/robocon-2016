#ifndef __MAIN_H
#define __MAIN_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "led.h"

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

//#define IMU_UPSLOPE
#define SENSOR_BAR_UPSLOPE

#ifdef IMU_UPSLOPE
	#ifdef SENSOR_BAR_UPSLOPE
		#error UPSLOPE MODE BOTH DEFINED
	#endif
#else
	#ifndef SENSOR_BAR_UPSLOPE
		#error UPSLOPE MODE NOT YET DEFINED
	#endif
#endif

//	PIN_ON_TOP = 		0
//	PIN_ON_LEFT = 	1
//	PIN_ON_BOTTOM =	2
//	PIN_ON_RIGHT = 	3
#define ORIENTATION_SETTING 2

//Use only for IMU_UPSLOPE only
//Sensor bar upslope uses independent PID
#define UP_SENSOR_BAR_TRUST 		15 //Scaled by 100
#define UP_SENSOR_BAR_POWER 		3
#define UP_SENSOR_BAR_Kp 				350 //Scaled by 100
#define UP_SENSOR_BAR_ON 				70
#define UP_SENSOR_BAR_OFF 			20

#define DOWN_SENSOR_BAR_TRUST 	100 //Scaled by 100
#define DOWN_SENSOR_BAR_POWER 	1
#define DOWN_SENSOR_BAR_Kp 			250 //Scaled by 100

#define RIVER_SENSOR_BAR_TRUST 	100 //Scaled by 100
#define RIVER_SENSOR_BAR_POWER 	1
#define RIVER_SENSOR_BAR_Kp 		420 //Scaled by 100

#define PURE_SENSOR_BAR_POWER		2
#define PURE_SENSOR_BAR_Kp			350 //Scaled by 100

#include "stm32f4xx_gpio.h"
#include "tm_stm32f4_stdio.h"
#include "tm_stm32f4_gpio.h"
#include "servo.h"
#include "ticks.h"
#include "usart.h"
#include "tm_stm32f4_i2c.h"
#include "approx_math.h"
#include "gyro.h"
#include "can_protocol.h"
#include "can_motor.h"
#include "encoder.h"
#include "sensor_bar.h"
#include "ardu_imu_adp.h"
#include "MTi-1_UART.h"
#include "imu_targeting.h"
#include "button.h"
#include "buzzer.h"
#include "buzzer_song.h"
#include "menu.h"
#include "lcd_main.h"

//path files
#include "path_upslope_imu.h"
#include "path_upslope_sb.h"
#include "path_downslope.h"
#include "path_river.h"
#include "path_station.h"

#include "sb_cali.h"

//Main loop
extern u32 this_loop_ticks;
extern u32 last_loop_ticks;

//Larger loop
#define LONG_LOOP_TICKS 20
#define SHORT_LOOP_TICKS 3
extern u32 last_long_loop_ticks;
extern u32 last_short_loop_ticks;

//Different in ticks for any loop except main loop
extern u32 any_loop_diff;

#endif /* __MAIN_H */
