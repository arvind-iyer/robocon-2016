#include "semi_auto.h"

static s32 lm_start_X = 0, lm_start_Y = 0, lm_start_angle = 0, lm_start_ticks = 0;

static s32 last_angle_error = 0;
static s32 ap_start_X = 0, ap_start_Y = 0;

void limit_sa_approach_init(){
	ap_start_X = get_pos()->x;
	ap_start_Y = get_pos()->y;
}

inline static s32 get_ap_new_X(){
	return get_pos()->x - ap_start_X;
}

inline static s32 get_ap_new_Y(){
	return get_pos()->y - ap_start_Y;
}

static s32 lm_sa_app_sum_y_error = 0;
static s32 lm_sa_app_x_spped = 0;
static s32 lm_sa_app_y_spped = 0;
static bool gripper_ready = false;
u8 limit_sa_approach(s32 motor_vel[3]){
	s32 target_angle = 1800;
	s32 w = 0;
	
	if (abs(get_ap_new_X()) > LM_SA_APP_START_ROTATE_X){
		if (!gripper_ready){
			gripper_ready = true;
			
			gripper_down = true;
			gripper_extended = true;
			gripper_clawed = false;
			gripper_control(THIS_GRIPPER, gripper_down);
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			gripper_claw_control(THIS_GRIPPER, gripper_clawed);
		}
		
		//angle pid
		s32 this_angle_error =  abs(target_angle - get_angle())%3600;
		
		w = this_angle_error*LM_SA_ROTATE_P/1000 + (this_angle_error - last_angle_error)*LM_SA_ROTATE_D/1000;
		#ifdef BLUE_FIELD
			w = -w;
		#endif
		s32_cap(w, LM_SA_ROTATE_MAX, -LM_SA_ROTATE_MAX);
		last_angle_error = this_angle_error;
	}
	
	if (abs(get_ap_new_X()) > LM_SA_APP_START_ACCEL){
		if (abs(get_ap_new_X()) > LM_SA_APP_END_ACCEL){
			lm_sa_app_x_spped = LM_SA_APP_SLOW_SPEED;
		}else{
			lm_sa_app_x_spped = LM_SA_APP_SLOW_SPEED + (LM_SA_APP_FAST_SPEED - LM_SA_APP_SLOW_SPEED) 
												* (abs(abs(get_ap_new_X())) - LM_SA_APP_START_ACCEL) 
												/ (LM_SA_APP_END_ACCEL - LM_SA_APP_START_ACCEL);
		}
	}else{
		lm_sa_app_x_spped = LM_SA_APP_FAST_SPEED;
	}
	
	s32 target_new_y = (LM_SA_APP_CURVE_RADIUS - ((s32)u32_sqrt(abs(LM_SA_APP_CURVE_RADIUS*LM_SA_APP_CURVE_RADIUS 
											- (get_ap_new_X())*(get_ap_new_X())))))
											* LM_SA_APP_CURVE_GAIN /100;
	#ifdef RED_FIELD
		target_new_y = -target_new_y;
	#endif
	s32 y_error = get_ap_new_Y() - target_new_y;
	lm_sa_app_y_spped = (y_error * LM_SA_APP_Y_P + lm_sa_app_sum_y_error * LM_SA_APP_Y_I) /1000;
	lm_sa_app_sum_y_error += y_error;

	acc_update(lm_sa_app_x_spped, lm_sa_app_y_spped, w, BASE_ACC_CONSTANT, BASE_DEC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_DEC_CONSTANT);
	
	//If reached limit switch
	if (gpio_read_input(&PE8) || gpio_read_input(&PE9)){
		return 6;
	}
	
	return 5;
}

void limit_sa_init(){
	lm_start_X = get_pos()->x;
	lm_start_Y = get_pos()->y;
	lm_start_angle = get_angle();
	lm_start_ticks = this_loop_ticks;
}

inline static s32 get_lm_new_X(){
	return get_pos()->x - lm_start_X;
}

inline static s32 get_lm_new_Y(){
	return get_pos()->y - lm_start_Y;
}

inline static s32 get_lm_new_angle(){
	return get_angle() - lm_start_angle;
}

inline static s32 get_lm_passed_ticks(){
	return this_loop_ticks - lm_start_ticks;
}

static s32 buffer_start_ticks = 0;
u8 limit_sa_update(s32 motor_vel[3]){
	s32 w = 0;
	
	bool limit_switch_triggered[2] = {false};
	limit_switch_triggered[0] = gpio_read_input(&PE8);
	limit_switch_triggered[1] = gpio_read_input(&PE9);
	
	//Rotation
	if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
		w = LIMIT_ROTATE_BIGGER;
	}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
		w = -LIMIT_ROTATE_SMALLER;
	}
	
	s32 perpend_speed = 0;
	//Perpendicular
	if (limit_switch_triggered[0] || limit_switch_triggered[1]){
		perpend_speed = LIMIT_PERPEND_NORM;
	}else{
		perpend_speed = LIMIT_PERPEND_FAST;
	}
	
	//Parallel
	s32 parallel_speed = 0;
	
	acc_update(parallel_speed, -perpend_speed, w, BASE_ACC_CONSTANT, BASE_DEC_CONSTANT, ROTATE_ACC_CONSTANT, ROTATE_DEC_CONSTANT);
	
	if (limit_switch_triggered[0] && limit_switch_triggered[1]){
		if (buffer_start_ticks == 0){
			buffer_start_ticks = this_loop_ticks;
		}else if((this_loop_ticks - buffer_start_ticks) > LM_SA_CALI_TIME){
			limit_sa_init();
			return 7;
		}
	}else{
		buffer_start_ticks = 0;
	}
	return 6;
}

void sa_print_info(){
	tft_append_line("L%d %d %d %d",get_lm_new_X(), get_lm_new_Y(), get_lm_new_angle(), get_lm_passed_ticks());
}
