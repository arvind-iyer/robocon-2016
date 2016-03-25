#include "servo_targeting.h"

float target_yaw;
TM_SERVO_t dragon_servo;
bool using_sensor_bar = false;
u16 sensor_bar_trust = 0;
u8 power = 1;
//Scaled by 100
u16 sensor_bar_Kp = 100;

void servo_init(){
	TM_SERVO_Init(&dragon_servo, TIM3, TM_PWM_Channel_2, TM_PWM_PinsPack_1);
	TM_SERVO_SetDegrees(&dragon_servo, SERVO_MED_DEG);
}

void enable_sensor_bar(u16 new_sensor_bar_trust, u8 new_power, u16 new_sensor_bar_Kp){
	using_sensor_bar = true;
	sensor_bar_trust = new_sensor_bar_trust;
	power = new_power;
	sensor_bar_Kp = new_sensor_bar_Kp;
}

void disable_sensor_bar(){
	using_sensor_bar = false;
	sensor_bar_trust = 0;
	power = 1;
	sensor_bar_Kp = 100;
}

void force_set_angle(float angle){
	TM_SERVO_SetDegrees(&dragon_servo, angle);	
}

void set_target(float in_target_yaw){
	target_yaw = in_target_yaw;
}

float targeting_pid(float current_yaw){
	//Find the diffence in yaw
	//float yaw_diff = abs_diff(target_yaw, current_yaw);
	float yaw_diff = target_yaw - current_yaw;
	
	//Find the correction needed to the servo by the a curve where y=k*sqrt(x)
	float corr = -sqrtf(fabs(yaw_diff)) * SERVO_Kp * (yaw_diff>0?1.0f:-1.0f) / 1000.0f;
	//float corr = yaw_diff * SERVO_Kp / 1000;

	tft_println("TY:%d SV:%d", (int)(target_yaw*10),  (int)(dragon_servo.Degrees*10));
	
	float new_servo_deg = SERVO_MED_DEG;
	if (using_sensor_bar){
		new_servo_deg = SERVO_MED_DEG + corr + sensor_bar_get_corr(power, sensor_bar_Kp) * sensor_bar_trust /100;
	}else{
		new_servo_deg = SERVO_MED_DEG + corr;
	}
	//Limit the servo range
	return new_servo_deg>SERVO_MAX_DEG?SERVO_MAX_DEG : (new_servo_deg<SERVO_MIN_DEG?SERVO_MIN_DEG:new_servo_deg);
}

void targeting_update(float current_yaw){
	TM_SERVO_SetDegrees(&dragon_servo, targeting_pid(current_yaw));
}
