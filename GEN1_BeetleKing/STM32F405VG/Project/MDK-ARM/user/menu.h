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

#define NUMBER_OF_ITEMS 7

void menu_init(void);
GAME_STAGE menu_update(void);

#endif
