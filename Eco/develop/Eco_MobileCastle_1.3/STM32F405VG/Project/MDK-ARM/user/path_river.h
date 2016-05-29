#ifndef _PATH_RIVER_H
#define _PATH_RIVER_H

#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "lcd_main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "imu_targeting.h"
#include "sensor_bar.h"
#include "servo_interface.h"
#include "path.h"
#include "ardu_imu_adp.h"

#define IR_GPIO GPIOC
#define IR_1_Pin GPIO_Pin_0
#define IR_2_Pin GPIO_Pin_1

#define IR_BUFFER_LENGTH 8
#define ENCODER_AFTER_THIRD_ISLAND 6000
#define RIVER_SB_INC_PWM 4500
#define RIVER_SB_DEC_PWM 4500

extern s16 river_straight_yaw;

void path_river_init(s16 straight_yaw);
GAME_STAGE path_river_update(void);

#endif
