#include "manual_track.h"

s32 last_laser_diff = 0;
s32 laser_target_range_increment = 0;
u32 laser_range[2] = {0};
u32 laser_use_range[2] = {0};

static s32 curr_vx = 0, curr_vy = 0, curr_w = 0;;
static u16 accel_remainder = 0;
static u16 rotate_accel_remainder = 0;

bool laser_manual_update(s32 motor_vel[3], s32* rotate){
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
	
	//Decide the new laser_target_range_increment	
//	if (abs(get_pos()->x) < LASER_START_BACKING_OFF){
//		laser_target_range_increment = 0;
//	}else{
//		laser_target_range_increment = (abs(get_pos()->x)  - LASER_START_BACKING_OFF) * LASER_BACK_OFF_DISTANCE / (LASER_OFF_MIN_DISTANCE - LASER_START_BACKING_OFF) ;
//	}
	
	//Rotation
	//If laser_diff < 0, *rotate anti-clockwise
	s32 laser_diff = laser_use_range[1] - laser_use_range[0];
	*rotate = -(laser_diff * LASER_ROTATE_P / 1000) + (laser_diff - last_laser_diff) * LASER_ROTATE_D / 1000;
	*rotate = *rotate>350?350:(*rotate<-350?-350:*rotate);
	last_laser_diff = laser_diff;
	
	//Perpendicular
	u32 laser_avg = (laser_use_range[0] + laser_use_range[1])/2;
	s32 perpend_speed = 0;
	if (laser_avg>180){
		//If perpend_diff>0, move further away
		s32 perpend_diff = (LASER_TARGET_RANGE + laser_target_range_increment) - laser_avg;
		perpend_speed = perpend_diff * LASER_PERPEND_P / 1000;
	}else{
		//Instant escape
		perpend_speed = 300;
	}
	
	perpend_speed = s32_cap(perpend_speed, LASER_MAX_PARA_SPEED, -LASER_MAX_PARA_SPEED);
	
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
	
	if ((abs(get_pos()->x) > LASER_OFF_MIN_DISTANCE) && (laser_range[0] >= LASER_OUT_DISTANCE)){
		return false;
	}
	
	return true;
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
	
	if (gpio_read_input(POLE_LIMIT_SWITCH)){
		return 3;
	}
	
	bool limit_switch_triggered[2] = {false};
	limit_switch_triggered[0] = gpio_read_input(WALL_1_LIMIT_SWITCH);
	limit_switch_triggered[1] = gpio_read_input(WALL_2_LIMIT_SWITCH);
	
	#ifdef BLUE_FIELD
		//Rotation
		if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
			*rotate = LIMIT_ROTATE_BIGGER;
		}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
			*rotate = -LIMIT_ROTATE_SMALLER;
		}
	#else
		//Rotation
		if (!limit_switch_triggered[0] && limit_switch_triggered[1]){
			*rotate = -LIMIT_ROTATE_BIGGER;
		}else if(limit_switch_triggered[0] && !limit_switch_triggered[1]){
			*rotate = LIMIT_ROTATE_SMALLER;
		}
	#endif
		
	s32 rotate_accel_amount = ROTATE_ACCEL_CONSTANT + rotate_accel_remainder;
	rotate_accel_remainder = rotate_accel_amount % 1000;
	rotate_accel_amount /= 1000;
	if (Abs(*rotate-curr_w) < (rotate_accel_amount+1)){
		curr_w = *rotate;
	}else{
		if (*rotate > curr_w){
			curr_w += rotate_accel_amount;
		}else{
			curr_w -= rotate_accel_amount;
		}
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
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT + 
			(abs(get_new_Y()) - LIMIT_START_DECEL_Y)*(LIMIT_PARA_CONSTANT-LIMIT_PARA_SLOW_CONSTANT)/(LIMIT_END_DECEL_Y-LIMIT_START_DECEL_Y);
		}else{
			parallel_speed = LIMIT_PARA_SLOW_CONSTANT;
		}
	}else {
		parallel_speed = LIMIT_PARA_CONSTANT;
	}
		
	#ifdef BLUE_FIELD
		parallel_speed = parallel_speed;
		perpend_speed = perpend_speed;
	#else
		parallel_speed = -parallel_speed;
		perpend_speed = -perpend_speed;
	#endif
	
	u16 acceleration_amount;
	if ((parallel_speed*parallel_speed+perpend_speed*perpend_speed) > (curr_vx*curr_vx + curr_vy*curr_vy)){
		acceleration_amount = TRACK_ACC_CONSTANT + accel_remainder; //Scaled by 1000
	}else{
		acceleration_amount = TRACK_ACC_CONSTANT + accel_remainder; //Scaled by 1000
	}
	
	accel_remainder = acceleration_amount % 1000;
	acceleration_amount /= 1000;
	
	//If the difference is not that much, directly assign speed
	if (Abs(curr_vx - parallel_speed) < (acceleration_amount+1) && Abs(curr_vy - perpend_speed) < (acceleration_amount+1)){
		curr_vx = parallel_speed;
		curr_vy = perpend_speed;
	}else{
		if (curr_vx > parallel_speed){
			curr_vx -= acceleration_amount;
		}else{
			curr_vx += acceleration_amount;
		}
		if (curr_vy > perpend_speed){
			curr_vy -= acceleration_amount;
		}else{
			curr_vy += acceleration_amount;
		}
	}
	
	s32 curr_angle = int_arc_tan2(-curr_vy, curr_vx)*10;
	u32 curr_speed = u32_sqrt(curr_vy * perpend_speed + curr_vx * curr_vx);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	return 2;
}

static s32 last_angle_error = 0;
s32 river_rotate_update(s32 target){
	s32 this_angle_error = target - get_angle();
	this_angle_error = this_angle_error>1800?3600-this_angle_error:this_angle_error;
	this_angle_error = this_angle_error<-1800?3600+this_angle_error:this_angle_error;
	
	s32 pid_rotate = -(this_angle_error*RIVER_ROTATE_P/1000 + (this_angle_error - last_angle_error)*RIVER_ROTATE_D/1000);
	s32_cap(pid_rotate, RIVER_ROTATE_MAX, -RIVER_ROTATE_MAX);
	last_angle_error = this_angle_error;
	return pid_rotate;
}

