/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "led.h"
#include "lcd.h"
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
#include "line_sensor.h"
#include "buzzer_song.h"
#include "buzzer.h"
#include "infrared_sensor.h"

#define BAJAJ

#ifdef WILSON
#define SERVO_MICROS_MID 1150
#define SERVO_MICROS_RIGHT 550
#define SERVO_MICROS_LEFT 1750
#endif

#ifdef HITLER
#define SERVO_MICROS_MID 1500
#define SERVO_MICROS_RIGHT 2100
#define SERVO_MICROS_LEFT 900
#endif

#ifdef BAJAJ
#define SERVO_MICROS_MID 1300
#define SERVO_MICROS_RIGHT 1900
#define SERVO_MICROS_LEFT 700
#endif


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
