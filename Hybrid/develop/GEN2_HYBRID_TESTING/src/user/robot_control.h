#ifndef _ROBOT_CONTROL_H
#define _ROBOT_CONTROL_H

#include <stdbool.h>
#include "stm32f10x.h"
#include "servo.h"
#include "pneumatic.h"
#include "main.h"
#include "can_motor.h"
#include "xbc_control.h"

#define CLIMBING_SPEED 1500
#define DESCEND_SPEED -500
#define RAISE_ARM_SPEED 1000
#define LOWER_ARM_SPEED -1000

#define BRUSHLESS_MIN 450
#define BRUSHLESS_MAX 1050
#define BRUSHLESS_COUNT 1
#define BRUSHLESS_PORT SERVO3

#define BRUSHLESS_SERVO_PORT SERVO4
#define BRUSHLESS_SERVO_MED 760
#define BRUSHLESS_SERVO_RANGE 280

#define GRIPPER_MIN 400
#define GRIPPER_MED 660//680 //750
#define GRIPPER_MAX 855 //1100
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

typedef enum{
	BRUSHLESS_1 = 0,
	BRUSHLESS_2 = 1
}BRUSHLESS_ID;

typedef enum{
	GRIPPER_1 = 0,
	GRIPPER_2 = 1
}GRIPPER_ID;

void emergency_stop(void);

void brushless_control(u16 value, bool is_percentage_mode);
void brushless_servo_control(s16 value);

//state: 0 = down, 1 = upright
void gripper_control(GRIPPER_ID gripper_id, u8 state);
//state: 0 = retract, 1 = extend
void gripper_push_control(GRIPPER_ID gripper_id, u8 state);
//state: 0 = open, 1 = close
void gripper_claw_control(GRIPPER_ID gripper_id, u8 state);

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
