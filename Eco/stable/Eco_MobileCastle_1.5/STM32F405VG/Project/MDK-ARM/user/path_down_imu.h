#ifndef _PATH_DOWN_IMU_H
#define _PATH_DOWN_IMU_H

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
#include "path_downslope.h"

void coord_update(void);
s32 get_x(void);
s32 get_y(void);
s32 get_z(void);

#endif
