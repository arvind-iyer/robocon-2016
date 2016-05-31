#ifndef _MTI_ADP_H
#define _MTI_ADP_H

#include "stm32f4xx.h"
#include "ticks.h"
#include "led.h"
#include "main.h"
#include "imu_targeting.h"
#include <stdbool.h>
#include "path.h"

#include "path_upslope_imu.h"
#include "path_upslope_sb.h"

#include "MTi-1_UART.h"
#include "buzzer.h"
#include <math.h>

extern float mti_start_ypr[3];
extern s16 mti_int_ypr[3];
extern s16 mti_int_acc[3];
extern s16 mti_start_bias_yaw;

bool mti_all_ready(void);
void mti_init(void);
void mti_update(void);

#endif
