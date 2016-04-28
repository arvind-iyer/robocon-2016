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

#define IR_BUFFER_LENGTH 5

void path_river_init(void);
GAME_STAGE path_river_update(void);

#endif
