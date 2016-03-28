#ifndef _ARDU_IMU_ADP_H
#define _ARDU_IMU_ADP_H

#include "stm32f4xx.h"
#include "led.h"
#include "usart.h"
#include "main.h"
#include "servo_targeting.h"
#include <stdbool.h>
#include "path.h"
#include "path_upslope.h"
#include "ardu_imu.h"
#include "buzzer.h"
#include <math.h>

extern float ardu_start_ypr[3]; 
extern u16 ardu_int_ypr[3];

void ardu_adapter_init(void);
bool ardu_all_ready(void);
void ardu_imu_update(void);

#endif
