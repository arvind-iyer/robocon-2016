#ifndef __SYSTEM_H__
#define	__SYSTEM_H__

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
#include "button.h"
#include "encoder.h"

/*** Optional ***/
#include "can.h"
#include "can_protocol.h"
#include "can_motor.h"
#include "uart.h"
#include "approx_math.h"
#include "motor_pwm.h"
#include "servo.h"
#include "gyro.h"
#include "bluetooth.h"
#include "button.h"
#include "led.h"
#include "linear_ccd.h"
#include "adc.h"
#include "MPU6050.h"
#include "laser_sensor.h"

void system_init(void);
void ls_test(void);
void gyro_test(void);

#endif
