#ifndef _PATH_STATION_H
#define _PATH_STATION_H

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

#define STOPPED_RUNNING_TOL 2

void path_station_reset(void);
GAME_STAGE path_station_update(void);

#endif
