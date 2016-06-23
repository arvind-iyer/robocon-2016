#include "manual_track.h"

s32 last_laser_diff = 0;
s32 laser_target_range_increment = 0;
u32 laser_range[2] = {0};
u32 laser_use_range[2] = {0};

bool laser_manual_update(s16 motor_vel[3]){
	s16 w;
	
	//0 is the front one
	//1 is the back one
	for (s8 i=1;i>=0;i--){
		laser_range[i] = get_ls_cal_reading(i==0?FRONT_LASER_ID:BACK_LASER_ID);
		if (laser_range[i]>LASER_OUT_DISTANCE){
			laser_use_range[i] = LASER_TARGET_RANGE;
		}else{
			laser_use_range[i] = laser_range[i];
		}
	}
	
	//Rotation
	//If laser_diff < 0, *rotate anti-clockwise
	s16 laser_diff = laser_use_range[1] - laser_use_range[0];
	w = -(laser_diff * LASER_ROTATE_P / 1000) + (laser_diff - last_laser_diff) * LASER_ROTATE_D / 1000;
	w = s32_cap(w, 350, -350);
	last_laser_diff = laser_diff;
	
	//Perpendicular
	u32 laser_avg = (laser_use_range[0] + laser_use_range[1])/2;
	s16 perpend_speed = 0;
	if (laser_avg>180){
		//If perpend_diff>0, move further away
		s32 perpend_diff = (LASER_TARGET_RANGE + laser_target_range_increment) - laser_avg;
		perpend_speed = perpend_diff * LASER_PERPEND_P / 1000;
	}else{
		//Instant escape
		perpend_speed = 300;
	}
	
	perpend_speed = s16_cap(perpend_speed, LASER_MAX_PARA_SPEED, -LASER_MAX_PARA_SPEED);
	
	//Parallel (manual part)
	#ifdef RED_FIELD
		s32 parallel_input = xbc_get_joy(XBC_JOY_LX) - LASER_PARA_CONSTANT;
	#else
		s32 parallel_input = xbc_get_joy(XBC_JOY_LX) + LASER_PARA_CONSTANT;
	#endif
	s32 parallel_speed = parallel_input * LASER_PARA_P /1000;
	
	acc_update(-perpend_speed, parallel_speed, w, BASE_X_ACC_CONSTANT, BASE_X_DEC_CONSTANT, BASE_Y_ACC_CONSTANT, BASE_Y_DEC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_DEC_CONSTANT, false);
	
	if ((abs(get_pos()->x) > LASER_OFF_MIN_DISTANCE) && (laser_range[0] >= LASER_OUT_DISTANCE)){
		return false;
	}
	
	return true;
}

static u32 ls_hit_ticks = 0;
static bool pole_limit_hitted = false;
void limit_manual_init(){
	gyro_pos_set(0, 0, 0);
	ls_hit_ticks = 0;
	pole_limit_hitted = false;
}

//u8 limit_manual_update(s16 motor_vel[3]){
//	s16 w = 0;
//	
//	bool limit_switch_triggered[2] = {false};
//	limit_switch_triggered[0] = gpio_read_input(WALL_1_LIMIT_SWITCH);
//	limit_switch_triggered[1] = gpio_read_input(WALL_2_LIMIT_SWITCH);
//	
//	#ifdef BLUE_FIELD
//		//Rotation
//		if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
//			w = LIMIT_ROTATE_BIGGER;
//		}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
//			w = -LIMIT_ROTATE_SMALLER;
//		}
//	#else
//		//Rotation
//		if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
//			w = -LIMIT_ROTATE_BIGGER;
//		}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
//			w = LIMIT_ROTATE_SMALLER;
//		}
//	#endif
//	
//	s16 perpend_speed = 0;
//	//Perpendicular
//	if (limit_switch_triggered[0] || limit_switch_triggered[1]){
//		perpend_speed = LIMIT_PERPEND_NORM;
//	}else{
//		perpend_speed = LIMIT_PERPEND_FAST;
//	}
//	
//	//Parallel
//	s16 parallel_speed = 0;
//	if(abs(get_pos()->y)>LIMIT_START_DECEL_Y || pole_limit_hitted){
//		parallel_speed = LIMIT_PARA_SLOW_CONSTANT;
//	}else {
//		parallel_speed = LIMIT_PARA_CONSTANT;
//	}
//		
//	#ifdef BLUE_FIELD
//		parallel_speed = -parallel_speed;
//		perpend_speed = perpend_speed;
//	#else
//		parallel_speed = -parallel_speed;
//		perpend_speed = -perpend_speed;
//	#endif
//	
//	if (gpio_read_input(POLE_LIMIT_SWITCH)){
//		for (MOTOR_ID i=MOTOR1;i<=MOTOR3;i++){
//			motor_vel[i] = 0;
//			motor_loop_state[i] = OPEN_LOOP;
//			motor_set_vel(i, motor_vel[i], motor_loop_state[i]);
//		}
//		if (ls_hit_ticks == 0){
//			ls_hit_ticks = this_loop_ticks;
//		}
//	}
//	
//	if (ls_hit_ticks==0){
//		acc_update(-perpend_speed, parallel_speed, w, TRACK_ACC_CONSTANT, TRACK_ACC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_ACC_CONSTANT, false);
//	}else{
//		if((this_loop_ticks - ls_hit_ticks) > TICKS_AFTER_HIT_POLE){
//			return 3;
//		}
//	}
//	
//	return 2;
//}


u8 limit_gyro_update(s16 motor_vel[3]){
	s16 w = 0;
	
	bool limit_switch_triggered[2] = {false};
	limit_switch_triggered[0] = gpio_read_input(WALL_1_LIMIT_SWITCH);
	limit_switch_triggered[1] = gpio_read_input(WALL_2_LIMIT_SWITCH);
	
	w = (get_angle()>1800?get_angle()-3600:get_angle()) * LIMIT_GYRO_ROTATE_P / 100;
	
	s16 perpend_speed = 0;
	//Perpendicular
	if (limit_switch_triggered[0] || limit_switch_triggered[1]){
		perpend_speed = LIMIT_PERPEND_NORM;
	}else{
		perpend_speed = LIMIT_PERPEND_FAST;
	}
	
	//Parallel
	s16 parallel_speed = 0;
	if(abs(get_pos()->y)>LIMIT_START_DECEL_Y || pole_limit_hitted){
		parallel_speed = LIMIT_PARA_SLOW_CONSTANT;
	}else {
		parallel_speed = LIMIT_PARA_CONSTANT;
	}
		
	#ifdef BLUE_FIELD
		parallel_speed = -parallel_speed;
		perpend_speed = perpend_speed;
	#else
		parallel_speed = -parallel_speed;
		perpend_speed = -perpend_speed;
	#endif
	
	if (gpio_read_input(POLE_LIMIT_SWITCH)){
		pole_limit_hitted = true;
		if (ls_hit_ticks == 0){
			ls_hit_ticks = this_loop_ticks;
		}else	if ((this_loop_ticks - ls_hit_ticks) > TICKS_AFTER_HIT_POLE){
			return 3;
		}
	}else{
		ls_hit_ticks = 0;
	}
	
	acc_update(-perpend_speed, parallel_speed, w, TRACK_ACC_CONSTANT, TRACK_ACC_CONSTANT, TRACK_ACC_CONSTANT, TRACK_ACC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_ACC_CONSTANT, true);
	
	return 2;
}

