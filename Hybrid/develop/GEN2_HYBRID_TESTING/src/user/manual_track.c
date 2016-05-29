#include "manual_track.h"

s32 last_laser_diff = 0;
s32 laser_target_range_increment = 0;

void laser_manual_update(s32 motor_vel[3], s32* rotate){
	u32 laser_range[2];
	
	//0 is the front one
	//1 is the back one
	#ifdef BLUE_FIELD
		for (s8 i=0;i<2;i++){
			laser_range[i] = get_ls_cal_reading(i);
			if (laser_range[i]>650){
				laser_range[i] = LASER_TARGET_RANGE + laser_target_range_increment;
			}
		}
	#else
		for (s8 i=1;i>=0;i--){
			laser_range[i] = get_ls_cal_reading(i);
			if (laser_range[i]>650){
				laser_range[i] = LASER_TARGET_RANGE + laser_target_range_increment;
			}
		}
	#endif
	
	//Decide the new laser_target_range_increment	
	#ifdef RED_FIELD
	if ((-get_pos()->x) < LASER_START_BACKING_OFF){
	#else
	if ((get_pos()->x) < LASER_START_BACKING_OFF){
	#endif
		laser_target_range_increment = 0;
	}else{
		#ifdef RED_FIELD
		laser_target_range_increment = ((-get_pos()->x)  - LASER_START_BACKING_OFF) * LASER_BACK_OFF_DISTANCE / (LASER_TRACING_OFF_DISTANCE - LASER_START_BACKING_OFF) ;
		#else
		laser_target_range_increment = (get_pos()->x - LASER_START_BACKING_OFF) * LASER_BACK_OFF_DISTANCE / (LASER_TRACING_OFF_DISTANCE - LASER_START_BACKING_OFF);
		#endif
	}
	
	//Rotation
	//If laser_diff < 0, *rotate anti-clockwise
	s32 laser_diff = laser_range[0] - laser_range[1];
	*rotate = -(laser_diff * LASER_ROTATE_P / 1000) + (laser_diff - last_laser_diff) * LASER_ROTATE_D / 1000;
	*rotate = *rotate>350?350:(*rotate<-350?-350:*rotate);
	last_laser_diff = laser_diff;
	
	//Perpendicular
	u32 laser_avg = (laser_range[0] + laser_range[1])/2;
	s32 perpend_speed = 0;
	if (laser_avg>180){
		//If perpend_diff>0, move further away
		s32 perpend_diff = (LASER_TARGET_RANGE + laser_target_range_increment) - laser_avg;
		perpend_speed = perpend_diff * LASER_PERPEND_P / 1000;
	}else{
		//Instant escape
		perpend_speed = 300;
	}
	
	//Parallel (manual part)
	#ifdef RED_FIELD
		s32 parallel_input = xbc_get_joy(XBC_JOY_LX) - LASER_PARA_CONSTANT;
	#else
		s32 parallel_input = xbc_get_joy(XBC_JOY_LX) + LASER_PARA_CONSTANT;
	#endif
	s32 parallel_speed = parallel_input * LASER_PARA_P /1000;
	
	s32 curr_angle = int_arc_tan2(parallel_speed, -perpend_speed)*10;
	u32 curr_speed = u32_sqrt(perpend_speed * perpend_speed + parallel_speed * parallel_speed);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
}

static s32 start_X = 0, start_Y = 0, start_angle = 0;
static s32 start_ticks = 0;
void limit_manual_init(){
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
u8 limit_manual_update(s32 motor_vel[3], s32* rotate){
	
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

static s32 last_angle_error = 0;
s32 river_rotate_update(s32 target){
	s32 this_angle_error = target - get_angle();
	this_angle_error = this_angle_error>1800?3600-this_angle_error:this_angle_error;
	this_angle_error = this_angle_error<-1800?3600+this_angle_error:this_angle_error;
	
	s32 pid_rotate = -(this_angle_error*RIVER_ROTATE_P/1000 + (this_angle_error - last_angle_error)*RIVER_ROTATE_D/1000);
	last_angle_error = this_angle_error;
	return pid_rotate;
}

