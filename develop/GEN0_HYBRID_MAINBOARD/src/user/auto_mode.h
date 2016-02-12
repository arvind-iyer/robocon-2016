#ifndef	__AUTO_MODE_H
#define	__AUTO_MODE_H

#include "delay.h"
#include "lcd_main.h"
#include "system.h"
#include "stm32f10x.h"
#include "approx_math.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

void auto_tar_enqueue(int x, int y, int deg, int curve, bool stop);
void auto_tar_dequeue(void);
int auto_tar_queue_len(void);
int auto_get_ticks(void);
void auto_init(void);
void auto_reset(void);
bool auto_get_state(void);
void auto_menu_update(void);
void auto_var_update(void);
void auto_track_path(int angle, int rotate, int maxvel, bool curved);
void auto_motor_stop(void);
void auto_motor_update(void);
void auto_calibrate(void);

#endif
