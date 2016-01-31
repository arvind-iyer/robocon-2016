#include "manual_mode.h"

//Scaled by 10
s32 target_vx = 0;
s32 target_vy = 0;
s32 target_rotate = 0;
s32 curr_vx = 0;
s32 curr_vy = 0;
s32 curr_rotate = 0;
s32 curr_angle = 0;
s32 lastTicks = 0;
u16 brushless_time_count = 0;

LOCK_STATE brushless_lock = LOCKED;
LOCK_STATE emergency_lock = UNLOCKED;
LOCK_STATE pneumatic_state = UNLOCKED;
LOCK_STATE ground_wheels_lock = UNLOCKED;
LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
LOCK_STATE press_button_B = UNLOCKED;
LOCK_STATE press_button_back = UNLOCKED;
LOCK_STATE press_button_X = UNLOCKED;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	//Scaled by 10
	target_vx = target_vy = target_rotate = 0;
	curr_vx = curr_vy = curr_rotate = curr_angle = 0;
	lastTicks = 0;

	brushless_lock = LOCKED;
	emergency_lock = UNLOCKED;
	pneumatic_state = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = UNLOCKED;
	press_button_back = UNLOCKED;
	press_button_X = UNLOCKED;
}

void brushless_control(s16 value){
	if (value>BRUSHLESS_MAX){
		servo_control_all(BRUSHLESS_MAX);
	}else if(value<BRUSHLESS_MIN){
		servo_control_all(BRUSHLESS_MIN);
	}else{
		servo_control_all(value);
	}
	tft_append_line("%d", value);
}

void climb_continue(){
	if (UNLOCKED==emergency_lock){
		motor_set_vel(MOTOR4, CLIMBING_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR5, CLIMBING_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR6, CLIMBING_SPEED, OPEN_LOOP);
		climbing_induced_ground_lock = LOCKED;
	}
}

void descend_continue(){
	if (UNLOCKED==emergency_lock){
		motor_set_vel(MOTOR4, DESCEND_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR5, DESCEND_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR6, DESCEND_SPEED, OPEN_LOOP);
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
	tft_clear();
	
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
	
	//Only update ground wheels every 50 ms
	if (UNLOCKED==ground_wheels_lock && UNLOCKED==climbing_induced_ground_lock){
		if (get_full_ticks()-lastTicks > 50){
			lastTicks = get_full_ticks();
			led_blink(LED_D1);
			//Calcuate 3 base wheels movement
			s32 vx = xbc_get_joy(XBC_JOY_LX);
			s32 vy = xbc_get_joy(XBC_JOY_LY);
			//Plus 1800 to switch heading
			//curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 + 1800;
			curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
			if (abs(curr_vx-vx)<=2 && abs(curr_vy-vy)<=2){
				curr_vx = vx;
				curr_vy = vy;
			}else{
				curr_vx = (curr_vx*95/100 + vx*5/100);
				curr_vy = (curr_vy*95/100 + vy*5/100);
			}
			curr_rotate = xbc_get_joy(XBC_JOY_LT)*10/8 - xbc_get_joy(XBC_JOY_RT)*10/8;

			s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 300;
			s32 motor_vel[3] = {0};
			motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
			
			motor_set_vel(MOTOR1, motor_vel[0], CLOSE_LOOP);
			motor_set_vel(MOTOR2, motor_vel[1], CLOSE_LOOP);
			motor_set_vel(MOTOR3, motor_vel[2], CLOSE_LOOP);
		}
	}else{
		motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
	}
	tft_append_line("%d %d", curr_vx, curr_vy);
	tft_append_line("%d", get_ticks());
		
	//Calcuate brushless
	if (xbc_get_joy(XBC_JOY_RY)<-900 && button_pressed(BUTTON_XBC_R_JOY)){
		if (0 == brushless_time_count){
			brushless_time_count = get_seconds();
		}else if((get_seconds() - brushless_time_count)>=1){
			if (LOCKED == brushless_lock){
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
		if (UNLOCKED == brushless_lock){
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
	if (button_pressed(BUTTON_XBC_A) && UNLOCKED == emergency_lock){
		climb_continue();
		tft_append_line("CLIMBING");
	}else	if (button_pressed(BUTTON_XBC_Y) && UNLOCKED == emergency_lock){
		descend_continue();
		tft_append_line("DESCENDING");
	}else{
		stop_climbing();
		tft_append_line("STOPPING");
	}
	
	//Locking the motors
	if (button_pressed(BUTTON_XBC_X)){
		if (UNLOCKED == press_button_X){
			press_button_X = LOCKED;
			buzzer_beep(20);
			if (UNLOCKED == ground_wheels_lock){
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
		if (UNLOCKED == press_button_B && UNLOCKED == emergency_lock){
			press_button_B = LOCKED;
			pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
			pneumatic_state = !pneumatic_state;
		}
	}else{
		press_button_B = UNLOCKED;
	}
	
	//E-Stop
	if (button_pressed(BUTTON_XBC_BACK)){
		if (UNLOCKED == press_button_back){
			press_button_back = LOCKED;
			if (UNLOCKED == emergency_lock){
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
	tft_append_line("BL:%d  EM:%d", brushless_lock, emergency_lock);
	tft_append_line("GL:%d  CL:%d", ground_wheels_lock, climbing_induced_ground_lock);
	tft_update();
	return MANUAL_CONTROL;
}
