#include "manual_mode.h"

//Scaled by 10
s32 target_vx = 0;
s32 target_vy = 0;
s32 target_rotate = 0;
s32 curr_vx = 0;
s32 curr_vy = 0;
s32 curr_rotate = 0;
s32 curr_angle = 0;

LOCK_STATE is_climbing = LOCKED;
LOCK_STATE brushless_lock = UNLOCKED;
LOCK_STATE pneumatic_state = UNLOCKED;
LOCK_STATE ground_wheels_lock = UNLOCKED;
LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
LOCK_STATE press_button_B = UNLOCKED;
LOCK_STATE press_button_X = UNLOCKED;

u16 brushless_pressed_time = 0;

static XBC_JOY brushless_joy_sticks[2] = {XBC_JOY_LT, XBC_JOY_RT};
static u16 brushless_stick_max = 255;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void brushless_control(BRUSHLESS_ID brushless_id, u16 value, bool is_percentage_mode){
	if ((u8) brushless_id >= BRUSHLESS_COUNT) return;
	if (is_percentage_mode){
		value = value>100?100:value;
		servo_control((SERVO_ID)brushless_id, value);
	}else{
		value = value>BRUSHLESS_MAX?BRUSHLESS_MAX:(value<BRUSHLESS_MIN?BRUSHLESS_MIN:value);
		servo_control((SERVO_ID)brushless_id, (BRUSHLESS_MAX-BRUSHLESS_MIN)*value/100 + BRUSHLESS_MIN);
	}
}

void brushless_control_all(u16 value, bool is_percentage_mode){
	if (is_percentage_mode){
		value = value>100?100:value;
	}else{
		value = value>BRUSHLESS_MAX?BRUSHLESS_MAX:(value<BRUSHLESS_MIN?BRUSHLESS_MIN:value);
	}
	for (u8 i=0; i<BRUSHLESS_COUNT; i++){
		brushless_control((BRUSHLESS_ID)i, value, true);
	}
}

void manual_reset(){
	//Scaled by 10
	target_vx = target_vy = target_rotate = 0;
	curr_vx = curr_vy = curr_rotate = curr_angle = 0;

	brushless_lock = UNLOCKED;
	pneumatic_state = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = UNLOCKED;
	press_button_X = UNLOCKED;
	brushless_control_all(0, true);
}

void climb_continue(){
	motor_set_vel(MOTOR4, CLIMBING_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR5, CLIMBING_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR6, CLIMBING_SPEED, OPEN_LOOP);
}

void descend_continue(){
	motor_set_vel(MOTOR4, DESCEND_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR5, DESCEND_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR6, DESCEND_SPEED, OPEN_LOOP);
}

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
}

void manual_fast_update(){
	if (ground_wheels_lock == LOCKED){
		_lockInTarget();
	}
}

void manual_interval_update(){
	tft_clear();
	
	if (ground_wheels_lock == UNLOCKED && climbing_induced_ground_lock == UNLOCKED){
		//Calcuate 3 base wheels movement
		s32 vx = xbc_get_joy(XBC_JOY_LX);
		s32 vy = xbc_get_joy(XBC_JOY_LY);
		//Plus 1800 to switch heading
		//curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 + 1800;
		curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
		curr_vx = (curr_vx*95/100 + vx*5/100);
		curr_vy = (curr_vy*95/100 + vy*5/100);
		curr_rotate = -xbc_get_joy(XBC_JOY_RX)/3;

		s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 300;
		s32 motor_vel[3] = {0};
		motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
		motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
		motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
		
		for (u8 i=0;i<3;i++){
			//motor_set_vel(MOTOR1 + i, motor_vel[i], motor_vel[i]==0?OPEN_LOOP:CLOSE_LOOP);
			motor_set_vel(MOTOR1 + i, motor_vel[i], CLOSE_LOOP);
		}
	}
	
	tft_append_line("%d", this_loop_ticks);
	
	//Calcuate brushless
	if (brushless_lock == UNLOCKED){
		if (button_pressed(BUTTON_XBC_LB) && button_pressed(BUTTON_XBC_RB)){
			//Run the brushless
			//tft_append_line("B:%d %d", xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
			//brushless_control(BRUSHLESS_1, xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
			//brushless_control(BRUSHLESS_2, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);

			/* Control Manual: 
			** first 30% -> No respond
			** 30% ~ 50% -> Able to make eco start moving (constant across)
			** 50% ~ 100% -> Linear incrase up to 50% pwm
			** Keep 100% -> Continue grow to max power
			*/
			for (u8 i=0;i<BRUSHLESS_COUNT;i++){
				if (xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max*3/10)){
					brushless_pressed_time = 0;
					brushless_control((BRUSHLESS_ID)i, 0, true);
					
				}else if(xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max/2)){
					brushless_pressed_time = 0;
					brushless_control((BRUSHLESS_ID)i, 35, true);
					
				}else{
					//Cap the brushless_pressed_time to avoid overflow
					u16 ticks_different = (this_loop_ticks - last_long_loop_ticks);
					if ((u32) (brushless_pressed_time + ticks_different) < 65535){
						brushless_pressed_time += ticks_different;
					}else{
						brushless_pressed_time = 65535;
					}
					
					//If less than 1.2 seconds, keep 50%
					if (brushless_pressed_time<1200){
						brushless_control((BRUSHLESS_ID)i, 50, true);
					}else{
						u16 Iamjustatempvariable = 50 + (brushless_pressed_time-1200)/100;
						brushless_control((BRUSHLESS_ID)i, Iamjustatempvariable>100?100:Iamjustatempvariable, true);
					}
				}
			}
		}else{
			brushless_control_all(0, true);
		}
	}
	
	//Calcuate climbing wheels
	if (button_pressed(BUTTON_XBC_A)){
		climb_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("CLIMBING");
	}else	if (button_pressed(BUTTON_XBC_Y)){
		descend_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("DESCENDING");
	}else{
		stop_climbing();
		climbing_induced_ground_lock = UNLOCKED;
		tft_append_line("STOPPING");
	}
	
	//Locking the motors
	//TODO: Add PID locking
	if (button_pressed(BUTTON_XBC_X)){
		if (press_button_X == UNLOCKED){
			press_button_X = LOCKED;
			buzzer_beep(75);
			if (ground_wheels_lock == UNLOCKED){
				ground_wheels_lock = LOCKED;
				_setCurrentAsTarget();
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
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
			pneumatic_state = !pneumatic_state;
		}
	}else{
		press_button_B = UNLOCKED;
	}
	
	tft_append_line("BL:%d", brushless_lock);
	tft_append_line("GL:%d  CL:%d", ground_wheels_lock, climbing_induced_ground_lock);
	tft_update();
}

void manual_emergency_stop(){
	//E-Stop
	tft_append_line("S");
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
	motor_lock(MOTOR4);
	motor_lock(MOTOR5);
	motor_lock(MOTOR6);
	brushless_lock = LOCKED;
	ground_wheels_lock = LOCKED;
	brushless_control_all(0, true);
	tft_append_line("E");
	tft_update();
}

void manual_emergency_relax(){
	manual_reset();
}