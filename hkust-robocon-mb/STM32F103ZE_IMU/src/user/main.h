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

#endif /* __MAIN_H */
