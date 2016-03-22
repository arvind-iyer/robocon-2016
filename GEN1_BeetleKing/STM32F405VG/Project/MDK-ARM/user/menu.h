#ifndef _MENU_H
#define _MENU_H

#include "stm32f4xx.h"
#include "lcd_main.h"
#include "main.h"
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_ITEMS 7

void menu_init(void);
void menu_update(void);

#endif
