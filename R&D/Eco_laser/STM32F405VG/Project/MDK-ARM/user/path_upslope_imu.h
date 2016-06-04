#ifndef _PATH_UP_IMU_H
#define _PATH_UP_IMU_H

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
#include "quick_math.h"
#include "angle_range.h"

#define MAX(a, b) (a<b)?a:b
#define MIN(a, b) (a<b)?a:b

#define ROLLING_PITCH_SIZE 5
#define PROGRESS_TICKS_MAX 150

void path_up_imu_init(u8 stage);
GAME_STAGE path_up_imu_update(void);

#endif