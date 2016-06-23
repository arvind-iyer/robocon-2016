#ifndef _SB_CALI_H
#define _SB_CALI_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "main.h"
#include <stdbool.h>
#include <string.h>
#include "path.h"
#include "buzzer.h"
#include "sensor_bar.h"
#include "comm.h"

#define NUMBER_OF_COLOR 6

GAME_STAGE sb_cali_update(void);

extern const char* colors_string[NUMBER_OF_COLOR];

#endif
