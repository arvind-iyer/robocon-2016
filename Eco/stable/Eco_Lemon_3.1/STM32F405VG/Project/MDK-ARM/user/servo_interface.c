/**
** This file is the interface for the main servo control.
** All servo control should be passed through here
** HKUST Robocon 2016
** Rex Cheng
**/

#include "servo_interface.h"

//All pwm and angle are scaled by 10
s16 current_servo_pwm = SERVO_MED_PWM;
s16 pending_servo_pwm = SERVO_MED_PWM;
s16 static_pwm_bias = 0;

//Init servo interface and set servo to middle position
void si_init(){
	current_servo_pwm = pending_servo_pwm = SERVO_MED_PWM;
	static_pwm_bias = 0;
	si_execute();
}

//Convert angle to pwm.
s16 angle_to_pwm(s16 angle){
	return angle*(SERVO_MAX_PWM-SERVO_MIN_PWM)/1800;
}

//Add bias in terms of angle
void si_set_st_deg_bias(s16 angle_bias){
	static_pwm_bias = angle_to_pwm(angle_bias);
}

//Add bias in terms of pwm
void si_set_st_pwm_bias(s16 pwm_bias){
	static_pwm_bias = pwm_bias;
}

//Add bias in terms of angle
void si_add_deg_bias(s16 angle_bias){
	pending_servo_pwm += angle_to_pwm(angle_bias);
}

//Add bias in terms of pwm
void si_add_pwm_bias(s16 pwm_bias){
	pending_servo_pwm += pwm_bias;
}

//Directly set the degree, static bias still here
void si_set_deg(u16 deg){
	pending_servo_pwm = angle_to_pwm(deg);
}

//Direcly set the pwm, static bias still here
void si_set_pwm(u16 pwm){
	pending_servo_pwm = pwm;
}

//Set the pending pwm to the servo
void si_execute(){
	current_servo_pwm = s16_cap(pending_servo_pwm, SERVO_MAX_PWM, SERVO_MIN_PWM);
	servo_control(DRAGON_SERVO, current_servo_pwm/10);
	pending_servo_pwm = SERVO_MED_PWM + static_pwm_bias;
}

//Clear all the bias except the static one
void si_clear(){
	pending_servo_pwm = SERVO_MED_PWM + static_pwm_bias;
}

//Clear the static bias
void si_clear_static(){
	static_pwm_bias = 0;
}
