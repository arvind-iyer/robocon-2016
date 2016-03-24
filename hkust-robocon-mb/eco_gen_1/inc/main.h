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
#include <stdbool.h>
#include "stm32f4xx.h"
#include "led.h"
#include "tft_display.h"
#include "stm32f4xx_gpio.h"
#include "usart.h"
#include "approx_math.h"
#include "gyro.h"
#include "can_protocol.h"
#include "can_motor.h"
#include "encoder.h"
#include "sensor_bar.h"
#include "line_sensor.h"
#include "buzzer_song.h"
#include "buzzer.h"
#include "infrared_sensor.h"
#include "ticks.h"
#include "servo.h"

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
#define HITLER_SERVO SERVO2
#endif

#ifdef BAJAJ
#define SERVO_MICROS_MID 1500
#define SERVO_MICROS_RIGHT 2200
#define SERVO_MICROS_LEFT 800
#define BAJAJ_SERVO  SERVO3
#endif


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
