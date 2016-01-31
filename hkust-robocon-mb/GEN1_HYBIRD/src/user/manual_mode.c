#include "manual_mode.h"

//Scaled by 10
s32 target_vx = 0;
s32 target_vy = 0;
s32 target_rotate = 0;
s32 curr_vx = 0;
s32 curr_vy = 0;
s32 curr_rotate = 0;
s32 curr_angle = 0;
u16 brushless_time_count = 0;

LOCK_STATE brushless_lock = LOCKED;
LOCK_STATE emergency_lock = UNLOCKED;
LOCK_STATE pneumatic_state = UNLOCKED;
LOCK_STATE ground_wheels_lock = UNLOCKED;
LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
LOCK_STATE press_button_B = UNLOCKED;
LOCK_STATE press_button_back = UNLOCKED;
LOCK_STATE press_button_X = UNLOCKED;

s32 diff[3] = {0};
s32 last_motor_vel[3] = {0};

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	//Scaled by 10
	target_vx = target_vy = target_rotate = 0;
	curr_vx = curr_vy = curr_rotate = curr_angle = 0;

	brushless_lock = LOCKED;
	emergency_lock = UNLOCKED;
	pneumatic_state = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = UNLOCKED;
	press_button_back = UNLOCKED;
	press_button_X = UNLOCKED;

	for (u8 i=0;i<3;i++){
		diff[i] = 0;
		last_motor_vel[i] = 0;
	}
}

void brushless_control(s16 value){
	if (value>BRUSHLESS_MAX){
		servo_control_all(BRUSHLESS_MAX);
		climbing_induced_ground_lock = LOCKED;
	}else if(value<BRUSHLESS_MIN){
		servo_control_all(BRUSHLESS_MIN);
		climbing_induced_ground_lock = LOCKED;
	}else{
		servo_control_all(value);
		climbing_induced_ground_lock = UNLOCKED;
	}
}

void climb_continue(){
	if (emergency_lock==UNLOCKED){
		motor_set_vel(MOTOR4, 1799, OPEN_LOOP);
		motor_set_vel(MOTOR5, 1799, OPEN_LOOP);
		motor_set_vel(MOTOR6, 1799, OPEN_LOOP);
		climbing_induced_ground_lock = LOCKED;
	}
}

void descend_continue(){
	if (emergency_lock==UNLOCKED){
		motor_set_vel(MOTOR4, -1200, OPEN_LOOP);
		motor_set_vel(MOTOR5, -1200, OPEN_LOOP);
		motor_set_vel(MOTOR6, -1200, OPEN_LOOP);
		climbing_induced_ground_lock = LOCKED;
	}
}

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
	climbing_induced_ground_lock = UNLOCKED;
}

CONTROL_STATE manual_control_update(){
	if (button_pressed(BUTTON_XBC_XBOX)){
		brushless_control(BRUSHLESS_MIN);
		
		motor_set_vel(MOTOR1, 0, OPEN_LOOP);
		motor_set_vel(MOTOR2, 0, OPEN_LOOP);
		motor_set_vel(MOTOR3, 0, OPEN_LOOP);
		motor_set_vel(MOTOR4, 0, OPEN_LOOP);
		motor_set_vel(MOTOR5, 0, OPEN_LOOP);
		motor_set_vel(MOTOR6, 0, OPEN_LOOP);
		buzzer_beep(1100);
		return AUTO_MODE;
	}
	
	tft_clear();
	if (ground_wheels_lock==UNLOCKED && climbing_induced_ground_lock==UNLOCKED){
		//Calcuate 3 base wheels movement
		s32 vx = xbc_get_joy(XBC_JOY_LX);
		s32 vy = xbc_get_joy(XBC_JOY_LY);
		//Plus 1800 to switch heading
		//curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 + 1800;
		curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
		curr_vx = (curr_vx*95/100 + vx*5/100);
		curr_vy = (curr_vy*95/100 + vy*5/100);
		curr_rotate = xbc_get_joy(XBC_JOY_LT)*10/8 - xbc_get_joy(XBC_JOY_RT)*10/8;

		s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 300;
		s32 motor_vel[3] = {0};
		motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
		motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
		motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
		
		for (u8 i=0;i<3;i++){
			//motor_set_vel(MOTOR1 + i, motor_vel[i], motor_vel[i]==0?OPEN_LOOP:CLOSE_LOOP);
			motor_set_vel(MOTOR1 + i, motor_vel[i], CLOSE_LOOP);
			if (diff[i]<((motor_vel[i]-last_motor_vel[i])*(motor_vel[i]-last_motor_vel[i]))){
				diff[i] = (motor_vel[i]-last_motor_vel[i])*(motor_vel[i]-last_motor_vel[i]);
			}
			last_motor_vel[i] = motor_vel[i];
		}
		
		tft_append_line("%d", curr_vx);
		tft_append_line("%d", curr_vy);
		tft_append_line("%d %d %d", get_pwm_value(MOTOR1), get_pwm_value(MOTOR2), get_pwm_value(MOTOR3));
		tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
		tft_append_line("%d %d %d", diff[0], diff[1], diff[2]);
	}else{
		motor_set_vel(MOTOR1, 0, OPEN_LOOP);
		motor_set_vel(MOTOR2, 0, OPEN_LOOP);
		motor_set_vel(MOTOR3, 0, OPEN_LOOP);
	}
	
	tft_append_line("%d", get_ticks());
	tft_update();
		
	//Calcuate brushless
	if (xbc_get_joy(XBC_JOY_RY)<-900 && button_pressed(BUTTON_XBC_R_JOY)){
		if (brushless_time_count == 0){
			brushless_time_count = get_seconds();
		}else if((get_seconds() - brushless_time_count)>=1){
			if (brushless_lock==LOCKED){
				//Press and long down to unlock brushless
				brushless_lock = UNLOCKED;
				brushless_control(BRUSHLESS_MIN);
				buzzer_beep(800);
			}else{
				//Press and long down to lock again
				brushless_lock = LOCKED;
				brushless_control(BRUSHLESS_MIN);
				buzzer_beep(350);
			}
			brushless_time_count = 65535;
		}
	}else{
		brushless_time_count = 0;
		//Run the brushless
		if (brushless_lock == UNLOCKED){
			if (button_pressed(BUTTON_XBC_R_JOY)){
				//More Power
				brushless_control(xbc_get_joy(XBC_JOY_RY)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/1000+BRUSHLESS_MIN);
			}else{
				//Less Power
				brushless_control(xbc_get_joy(XBC_JOY_RY)*(BRUSHLESS_MED_HIGH-BRUSHLESS_MIN)/1000+BRUSHLESS_MIN);
			}
		}else{
			brushless_control(BRUSHLESS_MIN);
		}
	}		
	//Calcuate climbing wheels
	if (button_pressed(BUTTON_XBC_A)){
		climb_continue();
	}else	if (button_pressed(BUTTON_XBC_Y)){
		descend_continue();
	}else{
		stop_climbing();
	}
	
	//Locking the motors
	if (button_pressed(BUTTON_XBC_X)){
		if (press_button_X == UNLOCKED){
			press_button_X = LOCKED;
			buzzer_beep(20);
			if (ground_wheels_lock == UNLOCKED){
				ground_wheels_lock = LOCKED;
				motor_lock(MOTOR1);
				motor_lock(MOTOR2);
				motor_lock(MOTOR3);
			}else {
				buzzer_beep(225);
				ground_wheels_lock = UNLOCKED;
			}
		}
	}else{
			press_button_X = UNLOCKED;
		}
	
	//Pneumatic control
	if (button_pressed(BUTTON_XBC_B)){
		if (press_button_B == UNLOCKED && emergency_lock == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
			pneumatic_state = !pneumatic_state;
		}
	}else{
		press_button_B = UNLOCKED;
	}
	
	//E-Stop
	if (button_pressed(BUTTON_XBC_BACK)){
		if (press_button_back == UNLOCKED){
			press_button_back = LOCKED;
			if (emergency_lock==UNLOCKED){
				motor_lock(MOTOR1);
				motor_lock(MOTOR2);
				motor_lock(MOTOR3);
				motor_lock(MOTOR4);
				motor_lock(MOTOR5);
				motor_lock(MOTOR6);
				brushless_lock = LOCKED;
				ground_wheels_lock = LOCKED;
				brushless_control(BRUSHLESS_MIN);
				emergency_lock = LOCKED;
				buzzer_beep(200);
			}else{
				brushless_lock = UNLOCKED;
				ground_wheels_lock = UNLOCKED;
				emergency_lock = UNLOCKED;
				buzzer_beep(500);
			}
		}
	}else{
			press_button_back = UNLOCKED;
	}
	
	return MANUAL_CONTROL;
}
