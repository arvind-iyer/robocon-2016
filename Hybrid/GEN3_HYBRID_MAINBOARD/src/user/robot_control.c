/**
** This file is to control the physical model of the robot
** It provides functions for both manual/auto control to call
**
**/

#include "robot_control.h"
s16 encoder_offset = 0;
u8 encoder_reset = 0;
LOCK_STATE pneumatic_state = UNLOCKED;

/**
** This part is for controlling brushless motors
**/
void brushless_control(u16 value, bool is_percentage_mode){
	if (get_emergency_lock() == LOCKED) return;
	if (is_percentage_mode){
		value = value>100?100:value;
		servo_control(BRUSHLESS_PORT, (BRUSHLESS_MAX-BRUSHLESS_MIN)*value/100 + BRUSHLESS_MIN);
	}else{
		value = value>BRUSHLESS_MAX?BRUSHLESS_MAX:(value<BRUSHLESS_MIN?BRUSHLESS_MIN:value);
		servo_control(BRUSHLESS_PORT, value);
	}
}

void brushless_control_pid(u16 pid_val) {
	if (get_PID_FLAG() == PID_ON) {
		set_tar_val(pid_val);
	} else {
		brushless_control((pid_val + 233.9f)/21.557f, true);
	}
}

void brushless_servo_control(s16 value){
	if (get_emergency_lock() == LOCKED) return;
	value = (value > BRUSHLESS_SERVO_ANGLE) ? BRUSHLESS_SERVO_ANGLE : ((value < -BRUSHLESS_SERVO_ANGLE) ? -BRUSHLESS_SERVO_ANGLE : value);
	u16 pwm_val = (value*(BRUSHLESS_SERVO_RANGE)/BRUSHLESS_SERVO_ANGLE)+BRUSHLESS_SERVO_MED;
	servo_control(BRUSHLESS_SERVO_PORT, pwm_val+BRUSHLESS_SERVO_OFFSET);
}

//Gripper control
void gripper_control(GRIPPER_ID gripper_id, GRIPPER_UP_STATE state) {
	if (state == GRIPPER_FULL_DOWN){
		if (gripper_id == GRIPPER_1) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_R_DOWN);
		} else if (gripper_id == GRIPPER_2) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_L_DOWN);
		}
	} else if (state == GRIPPER_FULL_UP) {
		if (gripper_id == GRIPPER_1) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_R_MED);
		} else if (gripper_id == GRIPPER_2) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_L_MED);
		}
	}else if (state == GRIPPER_HALF_UP){
		if (gripper_id == GRIPPER_1) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_R_HALF);
		} else if (gripper_id == GRIPPER_2) {
			servo_control((SERVO_ID)gripper_id, GRIPPER_L_HALF);
		}
	}
}

//state: 0 = extend, 1 = retract
void gripper_push_control(GRIPPER_ID gripper_id, u8 state){
	if (state == 0) {
		if (gripper_id == GRIPPER_1) {
			pneumatic_off(&GRIPPER_L_PUSH_PORT);
		} else if (gripper_id == GRIPPER_2) {
			pneumatic_off(&GRIPPER_R_PUSH_PORT);
		}
	} else if (state == 1) {
		if (gripper_id == GRIPPER_1) {
			pneumatic_on(&GRIPPER_L_PUSH_PORT);
		} else if (gripper_id == GRIPPER_2) {
			pneumatic_on(&GRIPPER_R_PUSH_PORT);
		}
	}
}

//state: 0 = open, 1 = close
void gripper_claw_control(GRIPPER_ID gripper_id, u8 state){
	if (state == 1) {
		if (gripper_id == GRIPPER_1) {
			pneumatic_off(&GRIPPER_L_CLAW_PORT);
		} else if (gripper_id == GRIPPER_2) {
			pneumatic_off(&GRIPPER_R_CLAW_PORT);
		}
	} else if (state == 0) {
		if (gripper_id == GRIPPER_1) {
			pneumatic_on(&GRIPPER_L_CLAW_PORT);
		} else if (gripper_id == GRIPPER_2) {
			pneumatic_on(&GRIPPER_R_CLAW_PORT);
		}
	}
}

//Brushless arm control
s16 get_arm_pos() {
	return (get_encoder_count(ENCODER1) - encoder_offset);
}

void raise_arm() {
	if (get_emergency_lock() == LOCKED) return;
	if (gpio_read_input(&ARM_UP_LIMIT_PORT)) {
		motor_set_vel(MOTOR7, 0, OPEN_LOOP);
		return;
	}
	motor_set_vel(MOTOR7, RAISE_ARM_SPEED*MOTOR7_FLIP, OPEN_LOOP);
}

void lower_arm() {
	if (get_emergency_lock() == LOCKED) return;
	if (gpio_read_input(&ARM_DN_LIMIT_PORT)) {
		motor_set_vel(MOTOR7, 0, OPEN_LOOP);
		if (encoder_reset == 0) {
			encoder_offset = get_encoder_count(ENCODER1);
			encoder_reset = 1;
		}
		return;
	} else {
		encoder_reset = 0;
	}
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

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
}

/**
** This part is for pneumatic control
**/

void slow_climb_continue(){
	if (get_emergency_lock() == LOCKED) return;
	motor_set_vel(MOTOR4, CLIMBING_SLOW_SPEED*MOTOR4_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR5, CLIMBING_SLOW_SPEED*MOTOR5_FLIP, OPEN_LOOP);
	motor_set_vel(MOTOR6, CLIMBING_SLOW_SPEED*MOTOR6_FLIP, OPEN_LOOP);
}

void pneumatic_climb_toggle(){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_state = (LOCK_STATE)!pneumatic_state;
	const GPIO * climb_port = &CLIMB_PNEUMATIC_PORT;
	pneumatic_control(climb_port->gpio, climb_port->gpio_pin, pneumatic_state);
}

void pneumatic_on(const GPIO* gpio){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_control(gpio->gpio, gpio->gpio_pin, LOCKED);
	pneumatic_state = LOCKED;
}

void pneumatic_off(const GPIO* gpio){
	if (get_emergency_lock() == LOCKED) return;
	pneumatic_control(gpio->gpio, gpio->gpio_pin, UNLOCKED);
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
	motor_lock(MOTOR7);
	manual_vel_set_zero();
	brushless_control(0, true);
}
