#ifndef _ACC_MODULE_H
#define _ACC_MODULE_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "approx_math.h"
#include "buzzer.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"
#include "quick_math.h"

#define MOTOR_MAX_MY_BOUND 150

extern s32 curr_vx, curr_vy, curr_w;
extern s32 motor_vel[3];
extern CLOSE_LOOP_FLAG motor_loop_state[3];

void acc_update(s32 vx, s32 vy, s32 w, s32 v_acc, s32 v_dec, s32 w_acc, s32 w_dec);

#endif
