#ifndef _IMU_H
#define _IMU_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include <stdbool.h>

#define IMU_UART COM1

extern bool synced;
extern u8 imu_buffer[12];
extern float yaw_pitch_roll[3]; 

//Warning: The current digital compass is disabled, to avoid the other teams use strong magnet
//#define IMU_USE_CONTINUOUS_MODE

void imu_init(void);
void imu_update(void);

#endif
