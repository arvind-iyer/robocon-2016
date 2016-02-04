#ifndef _PID_LOCK_H
#define _PID_LOCK_H

#include "approx_math.h"
#include "can_motor.h"
#include "gyro.h"
#include "lcd_main.h"

#define MAXVEL 140

void _move(int M, int dir, int W);
void _setTarget(int x, int y, int bearing);
void _setCurrentAsTarget(void);
void _straight(int x, int y, int bearing, int d_e, int a_e, int vel);
void _lockInTarget(void);

#endif