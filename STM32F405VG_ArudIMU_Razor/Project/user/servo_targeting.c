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
	float yaw_diff = current_yaw - target_yaw;
	yaw_diff = yaw_diff>180.0 ? yaw_diff-180.0 : (yaw_diff<-180.0 ? yaw_diff+180.0 : yaw_diff);
	
	float corr = sqrt(fabs(yaw_diff)) * SERVO_Kp * (yaw_diff>0?1:-1)/ 1000;

	tft_println("TY: %f", target_yaw);
	tft_println("YD: %f", yaw_diff);
	tft_println("CR: %f", corr);
	tft_println("SV: %f", dragon_servo.Degrees);
	
	float new_servo_deg = SERVO_MED_DEG + corr;
	return new_servo_deg>SERVO_MED_DEG?new_servo_deg : (new_servo_deg<SERVO_MIN_DEG?SERVO_MIN_DEG:new_servo_deg);
}

void targeting_update(float current_yaw){
	TM_SERVO_SetDegrees(&dragon_servo, targeting_pid(current_yaw));
}
