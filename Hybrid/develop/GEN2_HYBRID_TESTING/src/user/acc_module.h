#ifndef _ACC_MODULE_H
#define _ACC_MODULE_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "can_motor.h"
#include "approx_math.h"
#include "buzzer.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"
#include "quick_math.h"

#define MOTOR_MAX_MY_BOUND 150

extern s16 curr_vx, curr_vy, curr_w;
extern s16 motor_vel[3];
extern CLOSE_LOOP_FLAG motor_loop_state[3];

void acc_update(s16 vx, s16 vy, s16 w, s16 v_acc, s16 v_dec, s16 w_acc, s16 w_dec, bool global);

#endif
