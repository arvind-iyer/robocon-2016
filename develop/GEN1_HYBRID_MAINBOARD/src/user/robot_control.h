#ifndef _ROBOT_CONTROL_H
#define _ROBOT_CONTROL_H

#include <stdbool.h>
#include "stm32f10x.h"
#include "servo.h"
#include "pneumatic.h"
#include "main.h"
#include "can_motor.h"
#include "xbc_control.h"

#define CLIMBING_SPEED 1799
#define DESCEND_SPEED -500
#define RAISE_ARM_SPEED 1000
#define LOWER_ARM_SPEED -1000

#define BRUSHLESS_MIN 400
#define BRUSHLESS_MED_HIGH 800
#define BRUSHLESS_MAX 1050
#define BRUSHLESS_COUNT 2

#define GRIPPER_MIN 450
#define GRIPPER_MED 750
#define GRIPPER_MAX 1050
#define GRIPPER_COUNT 2

#define MOTOR4_FLIP 1
#define MOTOR5_FLIP 1
#define MOTOR6_FLIP -1
#define MOTOR7_FLIP 1

typedef enum{
	BRUSHLESS_1 = 0,
	BRUSHLESS_2 = 1
}BRUSHLESS_ID;

typedef enum{
	GRIPPER_1 = 0,
	GRIPPER_2 = 1
}GRIPPER_ID;

void emergency_stop(void);

void brushless_control(BRUSHLESS_ID brushless_id, u16 value, bool is_percentage_mode);
void brushless_control_all(u16 value, bool is_percentage_mode);

void gripper_control(GRIPPER_ID gripper_id, u16 state);

void raise_arm(void);
void lower_arm(void);
void stop_arm(void);

void climb_continue(void);
void descend_continue(void);
void stop_climbing(void);

void pneumatic_toggle(void);
void pneumatic_on(void);
void pneumatic_off(void);

#endif
