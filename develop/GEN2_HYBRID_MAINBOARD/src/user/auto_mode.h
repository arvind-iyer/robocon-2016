#ifndef	__AUTO_MODE_H
#define	__AUTO_MODE_H

#include "delay.h"
#include "lcd_main.h"
#include "system.h"
#include "stm32f10x.h"
#include "approx_math.h"
#include "robot_control.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
	NODE_STOP,
	NODE_PASS,
	NODE_END
} PATH_NODE_TYPE;

typedef enum {
	MENU_MODE,
	RUNNING_MODE
} PID_MODE;

typedef struct {
	u16 x;
	u16 y;
	u16 deg;
} COORD3;

typedef	struct {
	PATH_NODE_TYPE type;
	s32 x;
	s32 y;
	s32 deg;
	s32 curve;
} TARGET;

int pythag(int a1, int a2, int b1, int b2);

void auto_tar_enqueue(TARGET target);
void auto_tar_dequeue(void);
int auto_tar_queue_len(void);
TARGET auto_tar_ret(int id);

int auto_get_ticks(void);
void auto_init(void);
void auto_reset(void);
PID_MODE auto_get_state(void);
void auto_track_path(int angle, int rotate, int maxvel, bool curved);
void auto_motor_stop(void);
void auto_calibrate(void);
void auto_menu_update(void);
void auto_var_update(void);
void auto_motor_update(void);

#endif
