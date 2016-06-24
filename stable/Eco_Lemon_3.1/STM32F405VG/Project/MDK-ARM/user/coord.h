#ifndef _COORD_H
#define _COORD_H

#include "stm32f4xx.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void coord_update(void);
s32 get_x(void);
s32 get_y(void);
s32 get_z(void);

#endif
