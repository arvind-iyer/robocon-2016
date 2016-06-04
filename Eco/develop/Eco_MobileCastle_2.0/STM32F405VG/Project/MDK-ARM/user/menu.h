#ifndef _MENU_H
#define _MENU_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "main.h"
#include <stdbool.h>
#include <string.h>
#include "path.h"
#include "buzzer.h"

GAME_STAGE menu_update(GAME_STAGE current);

#endif
