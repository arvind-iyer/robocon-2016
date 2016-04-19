#include "servo_targeting.h"

float target_yaw;
TM_SERVO_t dragon_servo;

void servo_init(){
	TM_SERVO_Init(&dragon_servo, TIM3, TM_PWM_Channel_2, TM_PWM_PinsPack_1);
	TM_SERVO_SetDegrees(&dragon_servo, SERVO_MED_DEG);
}

void force_set_angle(float angle){
	TM_SERVO_SetDegrees(&dragon_servo, angle);	
}

void set_target(float in_target_yaw){
	target_yaw = in_target_yaw;
}

float targeting_pid(float current_yaw){
	//Find the diffence in yaw
	//Convert the yaw difference into a proper range
	//float yaw_diff = abs_diff(current_yaw, target_yaw);
	float yaw_diff = current_yaw - target_yaw;
	
	//Find the correction needed to the servo by the a curve where y=k*sqrt(x)
	float corr = sqrtf(fabs(yaw_diff)) * SERVO_Kp * (yaw_diff>0?1.0f:-1.0f) / 1000.0f;
	//float corr = yaw_diff * SERVO_Kp / 1000;

	tft_println("TY:%d SV:%d", (int)(target_yaw*10),  (int)(dragon_servo.Degrees*10));
	
	float new_servo_deg = SERVO_MED_DEG + corr;
	//Limit the servo range
	return new_servo_deg>SERVO_MED_DEG?new_servo_deg : (new_servo_deg<SERVO_MIN_DEG?SERVO_MIN_DEG:new_servo_deg);
}

void targeting_update(float current_yaw){
	TM_SERVO_SetDegrees(&dragon_servo, targeting_pid(current_yaw));
}
