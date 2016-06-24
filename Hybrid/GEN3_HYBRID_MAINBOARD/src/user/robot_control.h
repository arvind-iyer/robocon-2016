#ifndef _ROBOT_CONTROL_H
#define _ROBOT_CONTROL_H

typedef enum{
	BRUSHLESS_1 = 0,
	BRUSHLESS_2 = 1
}BRUSHLESS_ID;

typedef enum{
	GRIPPER_1 = 0,
	GRIPPER_2 = 1
}GRIPPER_ID;

typedef enum{
	GRIPPER_FULL_DOWN = 0,
	GRIPPER_HALF_UP = 1,
	GRIPPER_FULL_UP = 2
}GRIPPER_UP_STATE;

#include <stdbool.h>
#include "stm32f10x.h"
#include "servo.h"
#include "pneumatic.h"
#include "main.h"
#include "can_motor.h"
#include "xbc_control.h"

#define CLIMBING_SPEED 1500
#define DESCEND_SPEED -500
#define RAISE_ARM_SPEED 1350
#define LOWER_ARM_SPEED -1350

#define BRUSHLESS_MIN 450
#define BRUSHLESS_MAX 1050
#define BRUSHLESS_COUNT 1
#define BRUSHLESS_PORT SERVO3

#define BRUSHLESS_SERVO_PORT SERVO4
#define BRUSHLESS_SERVO_MED 740
#define BRUSHLESS_SERVO_RANGE 170
#define BRUSHLESS_SERVO_ANGLE 90
#define BRUSHLESS_SERVO_OFFSET 0

#define GRIPPER_L_MED 900
#define GRIPPER_R_MED 830
#define GRIPPER_INC 225
#define GRIPPER_L_DOWN (GRIPPER_L_MED + GRIPPER_INC) //855 //1100
#define GRIPPER_R_DOWN (GRIPPER_R_MED - GRIPPER_INC) //400
#define GRIPPER_L_HALF (GRIPPER_L_MED + GRIPPER_INC/2)
#define GRIPPER_R_HALF (GRIPPER_R_MED - GRIPPER_INC/2)

#define GRIPPER_COUNT 2
#define GRIPPER_R_PUSH_PORT PD11
#define GRIPPER_R_CLAW_PORT PD10
#define GRIPPER_L_PUSH_PORT PD9
#define GRIPPER_L_CLAW_PORT PD8

#define MOTOR4_FLIP 1
#define MOTOR5_FLIP 1
#define MOTOR6_FLIP -1
#define MOTOR7_FLIP 1

#define ARM_UP_LIMIT_PORT PE5
#define ARM_DN_LIMIT_PORT PE4
#define CLIMB_LIMIT_PORT PE3

#define CLIMB_PNEUMATIC_PORT PB9

void emergency_stop(void);

void brushless_control(u16 value, bool is_percentage_mode);
void brushless_control_pid(u16 pid_val);
void brushless_servo_control(s16 value);

void gripper_control(GRIPPER_ID gripper_id, GRIPPER_UP_STATE state);
//state: 0 = retract, 1 = extend
void gripper_push_control(GRIPPER_ID gripper_id, u8 state);
//state: 0 = open, 1 = close
void gripper_claw_control(GRIPPER_ID gripper_id, u8 state);

s16 get_arm_pos(void);
void raise_arm(void);
void lower_arm(void);
void stop_arm(void);

void climb_continue(void);
void descend_continue(void);
void stop_climbing(void);

void pneumatic_climb_toggle(void);
void pneumatic_on(const GPIO* gpio);
void pneumatic_off(const GPIO* gpio);

#endif
