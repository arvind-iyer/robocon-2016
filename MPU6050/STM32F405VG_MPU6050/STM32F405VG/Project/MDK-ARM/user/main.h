#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "stm32f4xx_gpio.h"
#include "tm_stm32f4_stdio.h"
#include "tm_stm32f4_timer_properties.h"
#include "tm_stm32f4_pwm.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_servo.h"
#include "tm_stm32f4_delay.h"
#include "usart.h"
#include "tm_stm32f4_i2c.h"
#include "approx_math.h"
#include "gyro.h"
#include "can_protocol.h"
#include "can_motor.h"
#include "encoder.h"
#include "sensor_bar.h"
#include "line_sensor.h"
#include "mpu6050.h"

//Main loop
extern u32 this_loop_ticks;
extern u32 last_loop_ticks;

//Larger loop
#define LONG_LOOP_TICKS 20
#define SHORT_LOOP_TICKS 5
extern u32 last_long_loop_ticks;
extern u32 last_short_loop_ticks;

//Different in ticks for any loop except main loop
extern u32 any_loop_diff;

void TimingDelay_Decrement(void);

#endif /* __MAIN_H */