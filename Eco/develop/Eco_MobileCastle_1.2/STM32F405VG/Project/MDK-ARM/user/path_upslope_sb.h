#ifndef _PATH_UP_SB_H
#define _PATH_UP_SB_H

#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "lcd_main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "sensor_bar.h"
#include "servo_interface.h"
#include "path.h"
#include "quick_math.h"
#include "encoder.h"

#define RIVER_90_TURN_CONSTANT 10000

void path_up_sb_init(u8 stage);
GAME_STAGE path_up_sb_update(void);

#endif
