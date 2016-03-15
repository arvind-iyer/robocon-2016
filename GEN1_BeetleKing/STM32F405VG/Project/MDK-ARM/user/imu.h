#ifndef _IMU_H
#define _IMU_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include "servo_targeting.h"
#include <stdbool.h>
#include "path_upslope.h"

#define IMU_UART COM1

typedef union {
	u8 chars[4];
	float f;
} byte2float;

extern bool imu_synced;
extern bool imu_staged;
extern u8 imu_buffer[12];
extern float out_ypr[3]; 
extern float cal_ypr[3]; 
extern float start_ypr[3];

#define IMU_USE_CONTINUOUS_MODE
#define SYNC_TIMEOUT 200

void imu_init(void);
void imu_update(void);

#endif
