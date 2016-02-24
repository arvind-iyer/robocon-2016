#ifndef _IMU_H
#define _IMU_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include <stdbool.h>

//Warning: The current digital compass is disabled, to avoid the other teams use strong magnet
//#define IMU_USE_CONTINUOUS_MODE

void imu_init(void);
void imu_update(void);

#endif