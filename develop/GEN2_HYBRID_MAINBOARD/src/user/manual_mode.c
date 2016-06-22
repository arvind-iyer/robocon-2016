/**
** This file is to handle the manual control part of the robot
**
** Control scheme:
** Left joy - Base Movement
** Right joy - Self rotation
** LB & RB - Unlock the brushless, will re-lock after 5 seconds if not used
** LT & RT - Control the speed of brushless motors respectively
** Button B - Set/Reset the pneumatic tube
** Button A & Y - Control the climbing/desending of the robot
** Button X - Lock the robot in place using pid
**/

#include "manual_mode.h"

static s32 curr_vx = 0, curr_vy = 0, curr_angle = 0;
static s32 curr_heading = 0;
static s32 actual_speed[2] = {0};
static s32 encoder_last_vel[2][ENCODER_READING_ARRAY_SIZE] = {0};
static u16	encoder_reading_pointer[2] = {0};

static s32 motor_vel[3] = {0};
static CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static bool is_rotating = false;

static LOCK_STATE brushless_lock = UNLOCKED;
static LOCK_STATE ground_wheels_lock = UNLOCKED;
static LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
static LOCK_STATE press_button_B = UNLOCKED;
static LOCK_STATE press_button_X = UNLOCKED;
static LOCK_STATE press_button_LB = UNLOCKED;
static LOCK_STATE press_button_RB = UNLOCKED;

static u16 brushless_pressed_time = 0;
static u16 brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT+1;
static XBC_JOY brushless_joy_sticks = XBC_JOY_RY;
static u16 brushless_stick_max = 1000;

static s16 brushless_servo_val = 0;
static u16 encoder_val = 0;

static BUTTON gripper_buttons[2] = {BUTTON_XBC_LB, BUTTON_XBC_RB};
static LOCK_STATE press_gripper_buttons[2] = {UNLOCKED};
static u16 gripper_states[2] = {0};
static u32 gripper_ticks[8] = {0}; //Left claw, Left push, Right claw, Right push

static s16 last_angle_pid = 0;
static s32 sum_of_last_angle_error = 0;

static u16 temp_control = 33;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	curr_vx = curr_vy = curr_angle = curr_heading = 0;
	brushless_lock = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = press_button_X = UNLOCKED;
	is_rotating = false;
	brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT + 1;
	brushless_control(0, true);
	brushless_servo_control(0);
	//gripper_control(GRIPPER_1, 0); 
	//gripper_control(GRIPPER_2, 0);
	pneumatic_off(&PD10);
	pneumatic_off(&PD11);
	set_tar_val(0);
	set_PID_FLAG(PID_OFF);
	servo_control(SERVO3, 450);
}

void manual_vel_set_zero(){
	for (u8 i=0;i<3;i++){
		motor_vel[i] = 0;
		motor_loop_state[i] = OPEN_LOOP;
	}
}

//Do PID in angle
s32 angle_pid(){
	s16 this_curr_global_angle = get_angle();
	s16 this_target_global_angle = curr_heading;
	//Map both angles to same area
	while(this_curr_global_angle>=1800){
		this_curr_global_angle -= 3600;
	}
	while(this_curr_global_angle<-1800){
		this_curr_global_angle += 3600;
	}
	while(this_target_global_angle>=1800){
		this_target_global_angle -= 3600;
	}
	while(this_target_global_angle<-1800){
		this_target_global_angle += 3600;
	}
	s32 error = this_target_global_angle - this_curr_global_angle;
	sum_of_last_angle_error = sum_of_last_angle_error*98/99 + error;
	//This value scaled by 1000(PID constant) and then 10(angle scale)
	s32 temp = error*ANGLE_PID_P + sum_of_last_angle_error*ANGLE_PID_I + (this_curr_global_angle-last_angle_pid)*ANGLE_PID_D;
	last_angle_pid = this_curr_global_angle;
	
	if (temp > ANGLE_PID_MAX){
		temp = ANGLE_PID_MAX;
	}else if (temp < -ANGLE_PID_MAX){
		temp = -ANGLE_PID_MAX;
	}
	return temp;
}

/*
** This fast update is for controlling things that require a high refresh rate
** It contains:
** - Locking itself in place
** - Recording encoder reading and speed
*/
void manual_fast_update(){
	if (ground_wheels_lock == LOCKED){
		//_lockInTarget();
		s32 curr_rotate = -angle_pid()/1500;
		for (u8 i=0;i<3;i++){
			motor_vel[i] = curr_rotate/10;
			motor_loop_state[i] = CLOSE_LOOP;
			motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
		}
	}
	
	//Recording encoder velocity, need to connect encoder to mainboard
	for (u8 counter=0;counter<2;counter++){
		//s32 encoder_reading = get_encoder_count((ENCODER)counter);
		//encoder_last_vel[counter][encoder_reading_pointer[counter]] = encoder_reading;
		//encoder_reading_pointer[counter] = (encoder_reading_pointer[counter]+1) % ENCODER_READING_ARRAY_SIZE;
		//actual_speed[counter] = encoder_reading - (encoder_reading_pointer[counter]+1) % ENCODER_READING_ARRAY_SIZE;
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	
	if (ground_wheels_lock == UNLOCKED){
		if (climbing_induced_ground_lock == UNLOCKED){
			//Calcuate 3 base wheels movement
			s32 vx = xbc_get_joy(XBC_JOY_LX);
			s32 vy = xbc_get_joy(XBC_JOY_LY);
			
			/*
			** This part calculate acceleration
			** It caps the max. acceleration for both x and y
			** By finding the axis with more difference(thus more acceleration), cap it
			** Then use the same proportion for the another axis to ensure the angle is correct
			*/
			u8 acceleration_amount;
			//If both x and y are at low speed, use low speed mode
			if (Abs(curr_vx) < LOW_SPEED_THRESHOLD && Abs(curr_vy) < LOW_SPEED_THRESHOLD){
				acceleration_amount = LOW_SPEED_ACC;
			}else {
				acceleration_amount = HIGH_SPEED_ACC;
			}
			
			//If the difference is not that much, directly assign speed
			if (Abs(curr_vx - vx) < (acceleration_amount+1) && Abs(curr_vy - vy) < (acceleration_amount+1)){
				curr_vx = vx;
				curr_vy = vy;
			
			//Else:
			//Use the axis with larger difference as the major consideration
			//The other axis simply follow the proportion
			}else if (Abs(curr_vx - vx) > Abs(curr_vy - vy)){
				//Use x-axis as major
				s32 proportion;
				if (curr_vx > vx){
					proportion = acceleration_amount *1000 / ((s32)curr_vx - vx);
					curr_vx -= acceleration_amount;
				}else{
					proportion = acceleration_amount *1000 / ((s32)vx - curr_vx);
					curr_vx += acceleration_amount;
				}
				
				curr_vy += (vy - curr_vy) *proportion /1000;
				
			}else{
				//Use y-axis as major
				s32 proportion;
				if (curr_vy > vy){
					proportion = acceleration_amount *1000 / ((s32)curr_vy - vy);
					curr_vy -= acceleration_amount;
				}else{
					proportion = acceleration_amount *1000 / ((s32)vy - curr_vy);
					curr_vy += acceleration_amount;
				}
				
				curr_vx += (vx - curr_vx) *proportion /1000;
			}
			
			curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
			s32 curr_rotate = (xbc_get_joy(XBC_JOY_LT)-xbc_get_joy(XBC_JOY_RT))*(1.6);
			//change heading for angle PID use
			if (curr_rotate == 0){
				if (is_rotating){
					is_rotating = false;
					curr_heading = get_angle();
				}
				curr_rotate = -angle_pid()/1500;
			}else{
				is_rotating = true;
			}

			s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 700;
			motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
			
			for (u8 i=0;i<3;i++){
				motor_loop_state[i] = CLOSE_LOOP;
			}
			tft_append_line("%d %d", curr_vx, curr_vy);
			tft_append_line("%d", curr_rotate);
			tft_append_line("%d %d %d", ((-1)*get_pos()->x), get_pos()->y, get_pos()->angle);
			//uart_tx(COM2, (uint8_t *)"%d, %d, %d\n", ((-1)*get_pos()->x), get_pos()->y, get_pos()->angle);
			
			//tft_append_line("GRIP %d %d", gripper_states[0], gripper_states[1]);
			//tft_append_line("LS %d %d", get_ls_cal_reading(0), get_ls_cal_reading(1));
			tft_append_line("LS %d %d", get_ls_cal_reading(0), get_ls_cal_reading(1));
			tft_append_line("DEG %d", brushless_servo_val);
			tft_append_line("RPM %d", get_diff());
			//tft_append_line("%d %d %d", get_target_vel(MOTOR1), get_target_vel(MOTOR2), get_target_vel(MOTOR3));
			//tft_append_line("%d %d %d", get_curr_vel(MOTOR1), get_curr_vel(MOTOR2), get_curr_vel(MOTOR3));
			//tft_append_line("%d %d %d", get_pwm_value(MOTOR1)/100 00, get_pwm_value(MOTOR2)/10000, get_pwm_value(MOTOR3)/10000);
		}else{
			for (u8 i=0;i<3;i++){
				motor_vel[i] = 0;
				motor_loop_state[i] = OPEN_LOOP;
			}
		}
	}
	
	tft_append_line("%d", this_loop_ticks);
	tft_append_line("%d", this_loop_ticks-last_long_loop_ticks);

	/*
	** This part deals with locking the robot.
	** When the lock button(X) is pressed, the ground wheels should be locked at least
	** For further implementation, it should lock itself in place with PID
	*/
	//TODO: Add PID locking
	if (button_pressed(BUTTON_XBC_X)){
		if (press_button_X == UNLOCKED){
			press_button_X = LOCKED;
			buzzer_beep(75);
			if (ground_wheels_lock == UNLOCKED){
				curr_heading = get_angle();
				ground_wheels_lock = LOCKED;
				_setCurrentAsTarget();
				manual_vel_set_zero();
			}else {
				buzzer_beep(225);
				ground_wheels_lock = UNLOCKED;
				manual_vel_set_zero();
			}
		}
	}else{
			press_button_X = UNLOCKED;
	}
	
	//At last apply the motor velocity and display it
	//Also happends in fast update
	for (u8 i=0;i<3;i++){
		motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
	}
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	tft_update();
}

void manual_controls_update(void) {
	u16 ticks_different = (this_loop_ticks - last_long_loop_ticks);
	
	/* 
	** This part provide the safety lock and control for the brushless motors
	**
	** Control Manual: 
	** first 30% -> No respond
	** 30% ~ 50% -> Able to make eco start moving (constant across)
	** 50% ~ 100% -> grow linearly to 50%
	** Keep 100% -> Continue grow to max power wih respect to time
	*/
	
	/*
	if (brushless_lock == UNLOCKED){
		//Run the brushless
		//tft_append_line("B:%d %d", xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
		//brushless_control(BRUSHLESS_1, xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
		//brushless_control(BRUSHLESS_2, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);

		//If not quite pressed, count time
		if (xbc_get_joy(brushless_joy_sticks[0]) < (brushless_stick_max/10) && xbc_get_joy(brushless_joy_sticks[1]) < (brushless_stick_max/10)){
				if ((brushless_lock_timeout + ticks_different) < BRUSHLESS_LOCK_TIMEOUT){
					brushless_lock_timeout += ticks_different;
				}else{
					brushless_lock = LOCKED;
					brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT + 1;
				}
		}else{
			brushless_lock_timeout = 0;
		}
			
		
		for (u8 i=0;i<BRUSHLESS_COUNT;i++){

			if (xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max*3/10)){
				brushless_pressed_time[i] = 0;
				brushless_control((BRUSHLESS_ID)i, 0, true);
				tft_append_line("%d%", 0);
				
			}else if(xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max/2)){
				brushless_pressed_time[i] = 0;
				brushless_control((BRUSHLESS_ID)i, 35, true);
				tft_append_line("%d%", 35);
				
			}else{
				//Cap the brushless_pressed_time to avoid overflow
				if ((brushless_pressed_time[i] + ticks_different) < 10000){
					brushless_pressed_time[i] += ticks_different;
				}else{
					brushless_pressed_time[i] = 10000;
				}
				
				//If less than 0.7 seconds, grow linearly to 50%
				if (brushless_pressed_time[i]<700){
					u16 Iamjustatempvariable = 35 + (xbc_get_joy(brushless_joy_sticks[i] - brushless_stick_max/2))*15/128;
					brushless_control((BRUSHLESS_ID)i, Iamjustatempvariable, true);
					tft_append_line("%d%", Iamjustatempvariable);
				}else{
					u16 Iamjustatempvariable = 50 + (brushless_pressed_time[i]-700)/20;
					Iamjustatempvariable = Iamjustatempvariable>100?100:Iamjustatempvariable;
					brushless_control((BRUSHLESS_ID)i, Iamjustatempvariable, true);
					tft_append_line("%d%", Iamjustatempvariable);
				}
			}
		}
	}else if (button_pressed(BUTTON_XBC_LB) && button_pressed(BUTTON_XBC_RB)){
		brushless_lock = UNLOCKED;
		brushless_lock_timeout = 0;
	}else{
		brushless_control_all(0, true);
		for (u8 i=0;i<BRUSHLESS_COUNT;i++){
			brushless_pressed_time[i] = 0;
		}
	}
	*/
	if (brushless_lock == UNLOCKED){

		//If not quite pressed, count time
		if (xbc_get_joy(brushless_joy_sticks) < (brushless_stick_max/10)){
				if ((brushless_lock_timeout + ticks_different) < BRUSHLESS_LOCK_TIMEOUT){
					brushless_lock_timeout += ticks_different;
				}else{
					brushless_lock = LOCKED;
					brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT + 1;
				}
		}else{
			brushless_lock_timeout = 0;
		}
		
		s16 brushless_pwm = xbc_get_joy(brushless_joy_sticks)/temp_control+20;
		if (brushless_pwm < 0)
			brushless_pwm = 0;
		brushless_control(brushless_pwm, true);
		tft_append_line("%d%", brushless_pwm);
		
	}else if (button_pressed(BUTTON_XBC_R_JOY)){
		brushless_lock = UNLOCKED;
		brushless_lock_timeout = 0;
	}else{
		brushless_control(0, true);
		brushless_pressed_time = 0;
	}
	
	// brushless arm
	if (button_pressed(BUTTON_XBC_E)){
		brushless_servo_val += BRUSHLESS_SERVO_STEP;
		if (brushless_servo_val > 140)
			brushless_servo_val = 140;
		brushless_servo_control(brushless_servo_val);
	}
	if (button_pressed(BUTTON_XBC_W)){
		brushless_servo_val -= BRUSHLESS_SERVO_STEP;
		if (brushless_servo_val < -140)
			brushless_servo_val = -140;
		brushless_servo_control(brushless_servo_val);
	}
	
	if (button_pressed(BUTTON_XBC_N)){
		raise_arm();
		tft_append_line("RAISING ARM");
	}else if (button_pressed(BUTTON_XBC_S)){
		lower_arm();
		tft_append_line("LOWERING ARM");
	}else {
		stop_arm();
	}
	encoder_val = get_encoder_count(ENCODER2);
	
	/*
	//gripper controls
	for (u8 i=0; i < GRIPPER_COUNT; i++) {
		if (button_pressed(gripper_buttons[i])) {
			if (press_gripper_buttons[i] == UNLOCKED) {
				press_gripper_buttons[i] = LOCKED;
				gripper_states[i] = (gripper_states[i] + 1) % GRIPPER_STATES_NO;
				
				switch (gripper_states[i]) {
					case 0: //Default state
						gripper_control((GRIPPER_ID)i, 0); //Down
						gripper_ticks[3+i*4] = get_full_ticks() + 1000; //pneumatic 2 hi; pushed out
						break;
					case 1: //Collect propeller
						gripper_ticks[0+i*4] = get_full_ticks() + 200; //pneumatic 1 lo; close claw
						gripper_ticks[2+i*4] = get_full_ticks() + 500; //pneumatic 2 lo; retract
						break;
					case 2: //Upright
						gripper_control((GRIPPER_ID)i, 1); //Up
						gripper_ticks[3+i*4] = get_full_ticks() + 1800; //pneumatic 2 hi; pushed out 
						break;
					case 3: //Chaiyo
						gripper_ticks[2+i*4] = get_full_ticks() + 200; //pneumatic 2 lo; retract
						gripper_ticks[1+i*4] = get_full_ticks() + 800; //pneumatic 1 hi; claw open 
						break;
				}
			}
		} else {
			press_gripper_buttons[i] = UNLOCKED;
		}
	}
	
	for (u8 i=0; i<8; i++) {
		if(Abs(get_full_ticks() - gripper_ticks[i]) <= GRIPPER_TICKS_THRESHOLD) {
			GPIO gripper_ticks_port;
			switch ((int)(i/2)) {
				case 0: //left claw
					gripper_ticks_port = PD10;
					break;
				case 1: //left push
					gripper_ticks_port = PD11;
					break;
				case 2: //right claw
					gripper_ticks_port = PD8;
					break;
				case 3: //right push
					gripper_ticks_port = PD9;
					break;
			}
			if (i%2) {
				pneumatic_off(&gripper_ticks_port);
			} else {
				pneumatic_on(&gripper_ticks_port);
			}
		}
	}
	*/
	
	/*
	** This part is related to climbing mechanism
	** Hold button A to climb up
	** Hold button Y to descend
	** While climbing, the ground wheels should be locked
	*/
	if (button_pressed(BUTTON_XBC_A) && !gpio_read_input(&PE3)){
		climb_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("CLIMBING");
	}else	if (button_pressed(BUTTON_XBC_Y)){
		/*
		descend_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("DESCENDING");
		*/
		temp_control = 20;
	}else{
		stop_climbing();
		climbing_induced_ground_lock = UNLOCKED;
	}
	
	/*
	** This part deals with locking the robot onto the pole.
	*/
	if (button_pressed(BUTTON_XBC_B)){
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_climb_toggle();
		}
	}else{
		press_button_B = UNLOCKED;
	}
}
