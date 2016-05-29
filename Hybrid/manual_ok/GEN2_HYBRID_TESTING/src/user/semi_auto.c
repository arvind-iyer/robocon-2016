#include "semi_auto.h"

static s32 start_X = 0, start_Y = 0, start_angle = 0;
static s32 start_ticks = 0;

static s32 last_angle_error = 0;
void limit_sa_approach(s32 target_angle){
	//angle pid
	s32 this_angle_error = target_angle - get_angle();
	this_angle_error = this_angle_error>1800?3600-this_angle_error:this_angle_error;
	this_angle_error = this_angle_error<-1800?3600+this_angle_error:this_angle_error;
	
	s32 pid_rotate = -(this_angle_error*RIVER_ROTATE_P/1000 + (this_angle_error - last_angle_error)*RIVER_ROTATE_D/1000);
	last_angle_error = this_angle_error;
}

void limit_sa_init(){
	start_X = get_pos()->x;
	start_Y = get_pos()->y;
	start_angle = get_angle();
	start_ticks = get_full_ticks();
}

static s32 get_new_X(){
	return get_pos()->x - start_X;
}

static s32 get_new_Y(){
	return get_pos()->y - start_Y;
}

static s32 get_new_angle(){
	return get_angle() - start_angle;
}

static s32 get_passed_ticks(){
	return get_full_ticks() - start_ticks;
}

static s32 last_Y = 0;
static u8 buffer = 0;
u8 limit_sa_update(s32 motor_vel[3], s32* rotate){
	
	if (abs(get_new_Y()-last_Y) < LIMIT_NO_Y_TOLER){
		buffer++;
	}else{
		buffer = 0;
	}
	last_Y = get_new_Y();
	if (abs(get_new_Y())>LIMIT_START_DECEL_Y && buffer>LIMIT_NO_Y_BUFFER){
		return 3;
	}
	
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
	if ((get_passed_ticks())<LIMIT_ACCEL_TIME){
		parallel_speed = (get_passed_ticks())*LIMIT_PARA_CONSTANT/LIMIT_ACCEL_TIME;
	}else if(abs(get_new_Y())>LIMIT_START_DECEL_Y){
		if (abs(get_new_Y())>LIMIT_END_DECEL_Y){
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT + (abs(get_new_Y()) - LIMIT_START_DECEL_Y)*(LIMIT_PARA_CONSTANT-LIMIT_PARA_SLOW_CONSTANT)/(LIMIT_END_DECEL_Y-LIMIT_START_DECEL_Y);
		}else{
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT;
		}
	}else {
		parallel_speed = LIMIT_PARA_CONSTANT;
	}
		
	#ifdef RED_FIELD
		parallel_speed = parallel_speed;
	#else
		parallel_speed = -parallel_speed;
	#endif
	
	s32 curr_angle = int_arc_tan2(-perpend_speed, parallel_speed)*10;
	u32 curr_speed = u32_sqrt(perpend_speed * perpend_speed + parallel_speed * parallel_speed);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	return 2;
}

