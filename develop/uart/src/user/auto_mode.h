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

typedef enum {
	NODE_STOP,
	NODE_PASS,
	NODE_END
} PATH_NODE_TYPE;

typedef enum {
	MENU_MODE,
	RUNNING_MODE,
	DATA_MODE
} PID_MODE;

typedef struct {
	u16 x;
	u16 y;
	u16 deg;
} COORD3;

typedef	struct {
	PATH_NODE_TYPE type;
	int x;
	int y;
	int deg;
	double curve;
} TARGET;

#include "auto_paths.h"

void auto_tar_enqueue(TARGET target);
int auto_tar_add_path(const TARGET* path);
void auto_tar_dequeue(void);
int auto_tar_queue_len(void);
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
void auto_data_update(void);

#endif