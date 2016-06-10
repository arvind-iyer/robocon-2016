#include "semi_auto.h"

static s32 lm_start_X = 0, lm_start_Y = 0, lm_start_angle = 0, lm_start_ticks = 0;

static s32 last_angle_error = 0;
static s32 ap_start_X = 0, ap_start_Y = 0, ap_start_angle = 0, ap_start_ticks;

void limit_sa_approach_init(){
	ap_start_X = get_pos()->x;
	ap_start_Y = get_pos()->y;
	ap_start_angle = get_angle();
	ap_start_ticks = get_full_ticks();
}

inline static s32 get_ap_new_X(){
	return get_pos()->x - ap_start_X;
}

inline static s32 get_ap_new_Y(){
	return get_pos()->y - ap_start_Y;
}

inline static s32 get_ap_new_angle(){
	return get_angle() - ap_start_angle;
}

inline static s32 get_ap_passed_ticks(){
	return this_loop_ticks - ap_start_ticks;
}

static s32 lm_sa_app_sum_y_error = 0;
static s32 lm_sa_app_x_spped = 0;
static s32 lm_sa_app_y_spped = 0;
u8 limit_sa_approach(s32 motor_vel[3], s32* rotate){
	s32 target_angle = 1800;
	
	//angle pid
	s32 this_angle_error = target_angle - get_angle();
	this_angle_error = this_angle_error>1800?3600-this_angle_error:this_angle_error;
	this_angle_error = this_angle_error<-1800?3600+this_angle_error:this_angle_error;
	
	*rotate = -(this_angle_error*LM_SA_ROTATE_P/1000 + (this_angle_error - last_angle_error)*LM_SA_ROTATE_D/1000);
	s32_cap(*rotate, LM_SA_ROTATE_MAX, -LM_SA_ROTATE_MAX);
	last_angle_error = this_angle_error;
	
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
	s32 y_error = get_ap_new_Y() - target_new_y;
	lm_sa_app_y_spped = (y_error * LM_SA_APP_Y_P + lm_sa_app_sum_y_error * LM_SA_APP_Y_I) /1000;
	lm_sa_app_sum_y_error += y_error;
	#ifdef BLUE_FIELD
		//lm_sa_app_y_spped = -lm_sa_app_y_spped;
		lm_sa_app_y_spped = 0;
		lm_sa_app_x_spped = -lm_sa_app_x_spped;
	#endif
	
	s32 curr_angle = int_arc_tan2(lm_sa_app_x_spped, lm_sa_app_y_spped)*10 - get_angle();
	u32 curr_speed = u32_sqrt((lm_sa_app_x_spped*lm_sa_app_x_spped) + (lm_sa_app_y_spped*lm_sa_app_y_spped));
	
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	
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
u8 limit_sa_update(s32 motor_vel[3], s32* rotate){
	
	bool limit_switch_triggered[2] = {false};
	limit_switch_triggered[0] = gpio_read_input(&PE8);
	limit_switch_triggered[1] = gpio_read_input(&PE9);
	
	//Rotation
	if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
		*rotate = LIMIT_ROTATE_BIGGER;
	}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
		*rotate = -LIMIT_ROTATE_SMALLER;
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
	s32 curr_angle = int_arc_tan2(-perpend_speed, parallel_speed)*10;
	u32 curr_speed = u32_sqrt(perpend_speed * perpend_speed + parallel_speed * parallel_speed);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	
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

static s32 last_Y = 0;
static u8 buffer = 0;
u8 sa_str_update(s32 motor_vel[3], s32* rotate){
	
	if (abs(get_lm_new_Y()-last_Y) < LIMIT_NO_Y_TOLER){
		buffer++;
	}else{
		buffer = 0;
	}
	last_Y = get_lm_new_Y();
	if (abs(get_lm_new_Y())>LIMIT_START_DECEL_Y && buffer>LIMIT_NO_Y_BUFFER){
		return 3;
	}
	
	s32 target_angle = 0;
	
	//angle pid
	s32 this_angle_error = target_angle - get_lm_new_angle();
	this_angle_error = this_angle_error>1800?3600-this_angle_error:this_angle_error;
	this_angle_error = this_angle_error<-1800?3600+this_angle_error:this_angle_error;
	*rotate = -this_angle_error * LM_SA_STR_ANGLE_P/1000;
	
	//Parallel
	s32 parallel_speed = 0;
	if ((get_lm_passed_ticks())<LIMIT_ACCEL_TIME){
		parallel_speed = (get_lm_passed_ticks())*LIMIT_PARA_CONSTANT/LIMIT_ACCEL_TIME;
	}else if(abs(get_lm_new_Y())>LIMIT_START_DECEL_Y){
		if (abs(get_lm_new_Y())>LIMIT_END_DECEL_Y){
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT + (abs(get_lm_new_Y()) - LIMIT_START_DECEL_Y)*(LIMIT_PARA_CONSTANT-LIMIT_PARA_SLOW_CONSTANT)/(LIMIT_END_DECEL_Y-LIMIT_START_DECEL_Y);
		}else{
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT;
		}
	}else {
		parallel_speed = LIMIT_PARA_CONSTANT;
	}
	
	s32 perpend_speed = get_lm_new_X()*LM_SA_STR_X_P/1000;
	s32 curr_angle = int_arc_tan2(perpend_speed, -parallel_speed)*10 - get_lm_new_angle();
	u32 curr_speed = u32_sqrt(perpend_speed * perpend_speed + parallel_speed * parallel_speed);
	
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	
	return 7;
}

void sa_print_info(){
	tft_append_line("L%d %d %d %d",get_lm_new_X(), get_lm_new_Y(), get_lm_new_angle(), get_lm_passed_ticks());
}
