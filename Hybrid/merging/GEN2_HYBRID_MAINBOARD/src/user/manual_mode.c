#include "manual_mode.h"

/**
* Control Scheme:
* Left joy stick for ground wheels movement
* Right joy stick for moving brushless arm and servo
* LB and RB for controlling brushless power, press once to inc./dec.
* LT and RT for rotation
*
* X to switch mode 0 <---> 1, rotate by 90, perpare gribber
* Back to activate auto go into pole mode
*
* In mode 0:
* Y - Activate/Deactivate laser tracking
* B - Rotating brushless servo by 90
*
* In mode 1:
* A - Claw pneumatic control
* B - Push pneumatic control
* Y - Whole gibber servo control
*
* The arm should rise automatically using IR at all times.
* Angel PID is disabled.
*/

static s32 curr_heading = 0;

static bool is_rotating = false;

static LOCK_STATE ground_wheels_lock = UNLOCKED;
static LOCK_STATE climbing_induced_ground_lock = UNLOCKED;

s16 brushless_servo_val = 0;
u16 brushless_power_percent = 20;
static u32 brushless_pressed_ticks = 0;
static u32 brushless_counted_ticks = 0;
static bool brushless_last_press_inc_flag = true;

static bool using_laser_sensor = true;
static bool pole_as_front = false;
static bool facing_pole = false;
static bool brushless_str = false;

bool gripper_extended = false;
bool gripper_clawed = true;
GRIPPER_UP_STATE gripper_down = GRIPPER_FULL_UP;

static s16 last_angle_pid = 0;
static s32 sum_of_last_angle_error = 0;

static u8 fast_per_long = 0;

/*
** 0 - Blowing mode
** 1 - Grabbing mode
** 2 - Auto go into pole mode
** 3 - Climb mode
** 4 - Putting propeller
** 5 - Semi-auto approach wall
** 6 - Semi-auto calibrate gyro and xy reading
** 7 - Semi-auto get propeller
*/
static u8 manual_stage = 0;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	curr_vx = curr_vy = curr_heading = curr_w = 0;
	manual_stage = 0;
	ground_wheels_lock = UNLOCKED;
	brushless_power_percent = 20;
	climbing_induced_ground_lock = UNLOCKED;
	is_rotating = pole_as_front = facing_pole = brushless_str = false;
	using_laser_sensor = true;
	gripper_clawed = false;
	gripper_extended = true;
	gripper_down = GRIPPER_FULL_DOWN;
	brushless_control(0, true);
	brushless_servo_control(0);
	brushless_servo_val = 0;
	pneumatic_off(&CLIMB_PNEUMATIC_PORT);
	for (GRIPPER_ID id=GRIPPER_1;id<=GRIPPER_2;id++){
		gripper_control(id, gripper_down);
		gripper_push_control(id, gripper_extended);
		gripper_claw_control(id, gripper_clawed);
	}
	for (MOTOR_ID i=MOTOR1;i<=MOTOR7;i++){
		motor_set_vel(i, 0, OPEN_LOOP);
	}
}

void manual_vel_set_zero(){
	curr_vx = curr_vy = curr_heading = curr_w = 0;
	for (u8 i=0;i<3;i++){
		motor_vel[i] = 0;
		motor_loop_state[i] = OPEN_LOOP;
	}
}

//Do PID in angle
s32 angle_pid(){
	s16 this_curr_global_angle = get_angle();
	s16 this_target_global_angle = curr_heading;
	s32 error = this_target_global_angle - this_curr_global_angle;
	
	if (error>1800){
		error = 3600 - error;
	}else if(error<-1800){
		error = 3600 + error;
	}
	
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


//This part deals with moving the robot around
void manual_update_wheel_base(bool use_laser_avoid){
	if (ground_wheels_lock == UNLOCKED){
		
		if (climbing_induced_ground_lock == UNLOCKED){
			s16	vx = xbc_get_joy(XBC_JOY_LX) *BASE_VEL_JOYSTICK_GAIN /1000;
			s16	vy = xbc_get_joy(XBC_JOY_LY) *BASE_VEL_JOYSTICK_GAIN /1000;
			s16 w = (xbc_get_joy(XBC_JOY_LT)-xbc_get_joy(XBC_JOY_RT))*8/5;
			
			if (use_laser_avoid){
				u32 avg_dis = (get_ls_cal_reading(FRONT_LASER_ID) + get_ls_cal_reading(BACK_LASER_ID))/2;
				if (avg_dis < LASER_RETREAT_AVG_DIS){
					//vy = -(LASER_RETREAT_SPEED + (LASER_RETREAT_AVG_DIS - avg_dis)*LASER_RETRAET_P/1000);
					vy = -LASER_RETREAT_SPEED;
					
				}else if(get_ls_cal_reading(FRONT_LASER_ID) < LASER_RETREAT_ONE_SIDE_DIS){
					//vy = -(LASER_RETREAT_SPEED + (LASER_RETREAT_ONE_SIDE_DIS - get_ls_cal_reading(FRONT_LASER_ID)*LASER_RETRAET_P/1000));
					vy = -LASER_RETREAT_SPEED;
					
				}else if(get_ls_cal_reading(BACK_LASER_ID) < LASER_RETREAT_ONE_SIDE_DIS){
					//vy = -(LASER_RETREAT_SPEED + (LASER_RETREAT_ONE_SIDE_DIS - get_ls_cal_reading(BACK_LASER_ID)*LASER_RETRAET_P/1000));
					vy = -LASER_RETREAT_SPEED;
				}
			}
			
			if (pole_as_front){
				acc_update(-vx, -vy, w, BASE_ACC_CONSTANT, BASE_DEC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_DEC_CONSTANT, false);
			}else{
				acc_update(vx, vy, w, BASE_ACC_CONSTANT, BASE_DEC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_DEC_CONSTANT, false);
			}
			
		}else{
			for (u8 i=0;i<3;i++){
				motor_vel[i] = 0;
				motor_loop_state[i] = OPEN_LOOP;
			}
		}
	}
}

/*
** This fast update is for controlling things that requires a high refresh rate
*/
void manual_fast_update(){
	fast_per_long++;
	ir_update();
	
	for (u8 i=0;i<3;i++){
		motor_vel[i] = 0;
	}
	 
	if (manual_stage == 5){
		manual_stage = limit_sa_approach(motor_vel);	
		
	}else if (manual_stage == 2){
		raise_arm();
		//manual_stage = limit_manual_update(motor_vel);		
		manual_stage = limit_gyro_update(motor_vel);
		curr_heading = get_angle();		
		
	}else if (manual_stage == 6){
		raise_arm();
		manual_stage = limit_sa_update(motor_vel);
		curr_heading = get_angle();
		
	}else if(manual_stage == 7){
		//manual_stage = sa_str_update(motor_vel, &curr_rotate);
		manual_stage = 1;
		curr_heading = get_angle();
		
	}else if(manual_stage == 3){
		climbing_update();
	}else if(manual_stage == 0 && using_laser_sensor){
		//using_laser_sensor = laser_manual_update(motor_vel, &curr_rotate);
		manual_update_wheel_base(true);
		curr_heading = get_angle();
		
	}else{
		manual_update_wheel_base(false);
		if (!is_rotating){
			//curr_rotate = -angle_pid()/1000;
		}
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	
	tft_append_line("%d", this_loop_ticks);
	tft_append_line("%d %d", this_loop_ticks-last_long_loop_ticks, fast_per_long);
	fast_per_long = 0;
	//sa_print_info();
	
	if (button_hitted[BUTTON_XBC_A]){
		if (manual_stage == 5 || manual_stage == 6){
			manual_stage = 1;
			using_laser_sensor = false;
			pole_as_front = true;
		}
	}
	
	if (button_hitted[BUTTON_XBC_X]){
		if (manual_stage == 0){
			manual_stage = 1;
			gripper_down = GRIPPER_FULL_DOWN;
			gripper_extended = true;
			gripper_clawed = false;
			gripper_control(THIS_GRIPPER, gripper_down);
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			gripper_claw_control(THIS_GRIPPER, gripper_clawed);
		}else{
			manual_stage = 0;
		}
		using_laser_sensor = false;
		pole_as_front = !pole_as_front;
		facing_pole = !facing_pole;
	}
			
	
	if (button_hitted[BUTTON_XBC_START]){
		buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
		limit_manual_init();
		manual_stage = 2;
		gripper_down = GRIPPER_HALF_UP;
		gripper_extended = true;
		gripper_clawed = true;
		gripper_control(THIS_GRIPPER, gripper_down);
		gripper_push_control(THIS_GRIPPER, gripper_extended);
		gripper_claw_control(THIS_GRIPPER, gripper_clawed);
	}
	
//	if (button_pressed(BUTTON_XBC_N)){
//		climb_continue();
//	}else{
//		stop_climbing();
//	}
	
	if (manual_stage == 0){
		manual_first_control_update();
	}else if(manual_stage == 1 || manual_stage == 5 || manual_stage == 6){
		manual_second_control_update();
	}
	
	//At last apply the motor velocity and display it
	//Value from fast update should also be displayed here
	tft_append_line("%d %d %d %d", get_pos()->x, get_pos()->y, get_angle());
	tft_append_line("%d %d %d %d", using_laser_sensor, manual_stage, facing_pole, brushless_str);
	tft_append_line("LR:%d %d %d", get_ls_cal_reading(FRONT_LASER_ID), get_ls_cal_reading(BACK_LASER_ID), get_ir_dis());
	tft_append_line("%d %d %d", curr_vx, curr_vy, curr_w);
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	tft_update();
}

void manual_first_control_update(){
	manual_control_brushless_update(); 
	
	//Pressing the back button to enable laser tracking
	if (button_hitted[BUTTON_XBC_Y]){
		buzzer_play_song(SUCCESSFUL_SOUND, 75, 0);
		using_laser_sensor = !using_laser_sensor;
	}
	
	if (button_hitted[BUTTON_XBC_A]){
		if (manual_stage == 0){
			manual_stage = 5;
			limit_sa_approach_init();
			using_laser_sensor = false;
			pole_as_front = true;
		}
	}
}

void manual_second_control_update() {
	manual_control_brushless_update();
	
	//Gripper servo
	if (button_hitted[BUTTON_XBC_Y]){
		buzzer_play_song(HIGH_BEEP, 125, 0);
		if (gripper_down == GRIPPER_FULL_DOWN){
			gripper_down = GRIPPER_FULL_UP;
		}else{
			gripper_down = GRIPPER_FULL_DOWN;
		}
		gripper_control(THIS_GRIPPER, gripper_down);
	}
	//Gripper push
	if (button_hitted[BUTTON_XBC_B]){
		buzzer_play_song(HIGH_BEEP, 125, 0);
		gripper_extended = !gripper_extended;
		gripper_push_control(THIS_GRIPPER, gripper_extended);
	}
	//Gripper claw
	if (button_hitted[BUTTON_XBC_A]){
		buzzer_play_song(HIGH_BEEP, 125, 0);
		gripper_clawed = !gripper_clawed;
		gripper_claw_control(THIS_GRIPPER, gripper_clawed);
	}
}

void manual_brushless_inc_step(){
	buzzer_play_song(BEEP, 75, 0);
	if (brushless_power_percent < 100){
		brushless_power_percent += BRUSHLESS_POWER_STEP;
	}
}

void manual_brushless_dec_step(){
	buzzer_play_song(BEEP, 75, 0);
	if (brushless_power_percent > 20){
		brushless_power_percent -= BRUSHLESS_POWER_STEP;
	}
}

void manual_control_brushless_update(){
	//Brushless power control, one hit
	if (button_hitted[BUTTON_XBC_RB]){
		brushless_last_press_inc_flag = true;
		brushless_pressed_ticks = this_loop_ticks;
		brushless_counted_ticks = 0;
		manual_brushless_inc_step();
	}
	
	if (button_hitted[BUTTON_XBC_LB]){
		brushless_last_press_inc_flag = false;
		brushless_pressed_ticks = this_loop_ticks;
		brushless_counted_ticks = 0;
		manual_brushless_dec_step();
	}
	
	//Brushless continuous control, i.e. holding
	if (button_pressed(BUTTON_XBC_RB) && brushless_last_press_inc_flag){
		if (((this_loop_ticks - brushless_pressed_ticks) > BRUSHLESS_START_CONTIN_TICKS) && brushless_counted_ticks==0){
			brushless_counted_ticks += BRUSHLESS_START_CONTIN_TICKS;
			manual_brushless_inc_step();
		}
		
		if ((this_loop_ticks - brushless_pressed_ticks) > (BRUSHLESS_KEEP_CONTIN_TICKS + brushless_counted_ticks)){
			brushless_counted_ticks += BRUSHLESS_KEEP_CONTIN_TICKS;
			manual_brushless_inc_step();
		}
	}else if(button_pressed(BUTTON_XBC_LB) && !brushless_last_press_inc_flag){
		if (((this_loop_ticks - brushless_pressed_ticks) > BRUSHLESS_START_CONTIN_TICKS) && brushless_counted_ticks==0){
			brushless_counted_ticks += BRUSHLESS_START_CONTIN_TICKS;
			manual_brushless_dec_step();
		}
		
		if ((this_loop_ticks - brushless_pressed_ticks) > (BRUSHLESS_KEEP_CONTIN_TICKS + brushless_counted_ticks)){
			brushless_counted_ticks += BRUSHLESS_KEEP_CONTIN_TICKS;
			manual_brushless_dec_step();
		}
	}
	
	brushless_control(brushless_power_percent, true);
	tft_append_line("%d", brushless_power_percent);
	
	// brushless arm
	if (xbc_get_joy(XBC_JOY_RX)>450){
		if (xbc_get_joy(XBC_JOY_RX)>900){
			brushless_servo_val += BRUSHLESS_SERVO_LARGE_STEP;
		}else{
			brushless_servo_val += BRUSHLESS_SERVO_SMALL_STEP;
		}
		if (brushless_servo_val > 140)
			brushless_servo_val = 140;
		brushless_servo_control(brushless_servo_val);
	}
	if (xbc_get_joy(XBC_JOY_RX)<-450){
		if (xbc_get_joy(XBC_JOY_RX)<-900){
			brushless_servo_val -= BRUSHLESS_SERVO_LARGE_STEP;
		}else{
			brushless_servo_val -= BRUSHLESS_SERVO_SMALL_STEP;
		}
		if (brushless_servo_val < -140)
			brushless_servo_val = -140;
		brushless_servo_control(brushless_servo_val);
	}
	
	if ((get_ir_dis() < IR_ORIGINAL_TARGET_DIS) && (xbc_get_joy(XBC_JOY_RY)>-100)){
		raise_arm();
	}else{
		if (xbc_get_joy(XBC_JOY_RY)>700){
			raise_arm();
			tft_append_line("RAISING ARM");
		}else if (xbc_get_joy(XBC_JOY_RY)<-700){
			lower_arm();
			tft_append_line("LOWERING ARM");
		}else {
			stop_arm();
		}
	}
}


void manual_controls_update() {
	manual_control_brushless_update();
}
