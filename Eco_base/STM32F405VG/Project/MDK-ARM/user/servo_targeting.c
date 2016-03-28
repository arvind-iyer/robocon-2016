#include "servo_targeting.h"

float target_yaw;
u16 current_pwm = 0;

bool using_sensor_bar = false;
u16 sensor_bar_trust = 0;
u8 power = 1;
//Scaled by 100
u16 sensor_bar_Kp = 100;

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
	servo_control(DRAGON_SERVO, angle);	
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

	tft_println("TY:%d SV:%d", (int)(target_yaw*10),  current_pwm);
	
	float new_servo_deg = SERVO_MED_PWM;
	if (using_sensor_bar){
		s16 correction = 0;
		SENSOR_BAR_FLAG flag = sensor_bar_get_corr(power, sensor_bar_Kp, &correction);
		new_servo_deg = SERVO_MED_PWM + corr + correction * sensor_bar_trust /100;
	}else{
		new_servo_deg = SERVO_MED_PWM + corr;
	}
	//Limit the servo range
	return new_servo_deg>SERVO_MAX_PWM?SERVO_MAX_PWM : (new_servo_deg<SERVO_MIN_PWM?SERVO_MIN_PWM:new_servo_deg);
}

void targeting_update(float current_yaw){
	current_pwm = targeting_pid(current_yaw);
	servo_control(DRAGON_SERVO, current_pwm);
}
