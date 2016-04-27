/**
** This file is to control the physical model of the robot
** It provides functions for both manual/auto control to call
**
**/

#include "robot_control.h"

LOCK_STATE pneumatic_state = UNLOCKED;

/**
** This part is for controlling brushless motors
**/
void brushless_control(BRUSHLESS_ID brushless_id, u16 value, bool is_percentage_mode){
	if (get_emergency_lock() == LOCKED) return;
	if ((u8) brushless_id >= BRUSHLESS_COUNT) return;
	if (is_percentage_mode){
		value = value>100?100:value;
		servo_control((SERVO_ID)brushless_id, (BRUSHLESS_MAX-BRUSHLESS_MIN)*value/100 + BRUSHLESS_MIN);
	}else{
		value = value>BRUSHLESS_MAX?BRUSHLESS_MAX:(value<BRUSHLESS_MIN?BRUSHLESS_MIN:value);
		servo_control((SERVO_ID)brushless_id, value);
	}
}

void brushless_control_all(u16 value, bool is_percentage_mode){
	if (get_emergency_lock() == LOCKED) return;
	if (is_percentage_mode){
		value = value>100?100:value;
	}else{
		value = value>BRUSHLESS_MAX?BRUSHLESS_MAX:(value<BRUSHLESS_MIN?BRUSHLESS_MIN:value);
	}
	for (u8 i=0; i<BRUSHLESS_COUNT; i++){
		brushless_control((BRUSHLESS_ID)i, value, true);
	}
}

//Brushless arm control
void raise_arm() {
	if (get_emergency_lock() == LOCKED) return;
	motor_set_vel(MOTOR7, RAISE_ARM_SPEED*MOTOR7_FLIP, OPEN_LOOP);
}

void lower_arm() {
	if (get_emergency_lock() == LOCKED) return;
	motor_set_vel(MOTOR7, LOWER_ARM_SPEED*MOTOR7_FLIP, OPEN_LOOP);
}

void stop_arm() {
	motor_set_vel(MOTOR7, 0, OPEN_LOOP);
}


/**
** This part is for climbing/descending
**/

void climb_continue(){
	if (get_emergency_lock() == LOCKED) return;
	motor_set_vel(MOTOR4, CLIMBING_SPEED*MOTOR4_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR5, CLIMBING_SPEED*MOTOR5_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR6, CLIMBING_SPEED*MOTOR6_FLIP, OPEN_LOOP);
}

void descend_continue(){
	if (get_emergency_lock() == LOCKED) return;
	motor_set_vel(MOTOR4, DESCEND_SPEED*MOTOR4_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR5, DESCEND_SPEED*MOTOR5_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR6, DESCEND_SPEED*MOTOR6_FLIP, OPEN_LOOP);
}

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
}

/**
** This part is for pneumatic control
**/

void pneumatic_toggle(){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_state = (LOCK_STATE)!pneumatic_state;
	pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
}

void pneumatic_on(){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_control(GPIOB, GPIO_Pin_9, LOCKED);
	pneumatic_state = LOCKED;
}

void pneumatic_off(){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_control(GPIOB, GPIO_Pin_9, UNLOCKED);
	pneumatic_state = UNLOCKED;
}

/**
** This part is for emergency locking
**/

void emergency_stop(){
	//E-Stop
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
	motor_lock(MOTOR4);
	motor_lock(MOTOR5);
	motor_lock(MOTOR6);
	manual_vel_set_zero();
	brushless_control_all(0, true);
}
