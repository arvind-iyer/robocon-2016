#include "manual_track.h"

void laser_manual_update(s32 motor_vel[3], s32* rotate){
	
	tft_clear();
	
	u32 laser_range[2];
	
	bool red_field = true;
	
	//0 is the front one
	//1 is the back one
	if (!red_field){
		for (s8 i=0;i<2;i++){
			laser_range[i] = get_ls_cal_reading(i);
		}
	}else{
		for (s8 i=1;i>=0;i--){
			laser_range[i] = get_ls_cal_reading(i);
		}
	}
	
	//Rotation
	//If laser_diff < 0, *rotate anti-clockwise
	s32 laser_diff = laser_range[0] - laser_range[1];
	*rotate = -laser_diff * LASER_ROTATE_P / 1000;
	*rotate = *rotate>350?350:(*rotate<-350?-350:*rotate);
	
	//Perpendicular
	u32 laser_avg = (laser_range[0] + laser_range[1])/2;
	s32 perpend_speed = 0;
	if (laser_avg>160){
		//If perpend_diff>0, move further away
		s32 perpend_diff = LASER_TARGET_RANGE - laser_avg;
		perpend_speed = perpend_diff * LASER_PERPEND_P / 1000;
	}else{
		//Instant escape
		perpend_speed = 200;
	}
	
	//Parallel (manual part)
	s32 parallel_input = xbc_get_joy(XBC_JOY_LY);
	s32 parallel_speed = parallel_input * LASER_PARA_P /1000;
	
	s32 curr_angle = int_arc_tan2(parallel_speed, -perpend_speed)*10;
	u32 curr_speed = u32_sqrt(perpend_speed * perpend_speed + parallel_speed * parallel_speed);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + *rotate)/10;
	
	s16 motor_vel_max = motor_vel[0];
	for (u8 i=1; i<3; i++){
		if (abs(motor_vel[i])>abs(motor_vel_max)){
			motor_vel_max = motor_vel[i];
		}
	}
	if (abs(motor_vel_max)>150){
		s32 motor_ratio = 150*10000/abs(motor_vel_max); //Scaled by 10000
		for (u8 i=0;i<3;i++){
			motor_vel[i] = (s32)motor_vel[i]*motor_ratio/10000;
		}
	}
	
	for (u8 i=0;i<3;i++){
		motor_set_vel((MOTOR_ID)MOTOR1+i, motor_vel[i], CLOSE_LOOP);
	}
	
	tft_append_line("%d %d", laser_range[0], laser_range[1]);
	tft_append_line("PR: %d %d", parallel_speed);
	tft_append_line("PP: %d %d", perpend_speed);
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	
	tft_update();
}
