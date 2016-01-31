#ifndef	__AUTO_MOVE_H
#define	__AUTO_MOVE_H

#include "delay.h"
#include "lcd_main.h"
#include "system.h"
#include "stm32f10x.h"
#include "approx_math.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

void auto_tar_enqueue(int x, int y, int deg, int curve, bool stop);
void auto_tar_dequeue();
int auto_tar_queue_len();

void auto_init();
void auto_var_update();
void auto_track_path(int angle, int rotate, int maxvel, bool curved);
void auto_motor_stop();
void auto_motor_update();
void auto_calibrate();

#endif