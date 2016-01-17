#include "goto.h"

int16_t target_x, target_y, target_angle;
uint8_t running = 0;
uint8_t stopping = 0;
int16_t Wheel_V[3] = {0};
int16_t track_m, track_c;
int16_t start_x, start_y, start_angle;
int16_t para_bearing;
int16_t k_value;
uint8_t y_index = 0;

uint8_t isRunning(){
	return running == 1;
}

int16_t normalise_angle(int16_t target){
	while (target>1800){
		target = 3600 - target;
	}
	while(target<-1800){
		target = 3600 + target;
	}
}

//Scaled by 100
#define Kp 10
#define Kd 100
int16_t lastEr = 0;
int16_t pid(int16_t errorDis){
	int16_t p_value = (Kp*(errorDis))/100;
	int16_t d_value = (Kd*(errorDis-lastEr))/100;

	tft_prints(0, y_index++, "%d %d", p_value, d_value);
	lastEr = errorDis;
//	if (errorDis<-50){
//		if (errorDis<-200){
//			if (errorDis<-500){
//				return 50;
//			}
//			return -10;
//		}
//		return -5;
//	}else if(errorDis>50){
//		if (errorDis>200){
//			if (errorDis>500){
//				return 50;
//			}
//			return 10;
//		}
//		return 5;
//	}else{
//		return 0;
//	}
	
	if (errorDis<-30 || errorDis>30){
		return p_value + d_value;
	}else{
		return 0;
	}
}

//Setup the target and values to help calculation
void setTargetPos(int16_t x, int16_t y, int16_t theta){
	start_x = get_X();
	start_y = get_Y();
	start_angle = get_angle();

	target_x = x;
	target_y = y;
	target_angle = normalise_angle(theta);

	//Scaled by 100
	track_m = (target_y - start_y)*100/(target_x - start_x);
	track_c = start_x*100 - track_m * start_y;
	k_value = (int16_t) sqrt(track_m*track_m+1*100*100);

	para_bearing = 900 - int_arc_tan2(target_y - start_y, target_x - start_x)*10 + start_angle;

	running = 1;
	stopping = 0;
}

#define TOTAL_STOP_COUNT 10
uint16_t stop_count = TOTAL_STOP_COUNT;
void setRobotVelocity(int16_t angle, int16_t velocity, int16_t w) {
	for (uint8_t i = 0; i<3; i++){
		int16_t temp = int_sin(angle + 1200 * i) * velocity * -1 / 10000 - w;
		if (temp-Wheel_V[i]>20){
			Wheel_V[i] += 20;
		}else if(temp-Wheel_V[i]<-20){
			Wheel_V[i] -= 20;
		}else{
			Wheel_V[i] = temp;
		}
	}

	if (stopping){
		for (uint8_t i=0;i<3;i++){
			Wheel_V[i] *= stop_count/TOTAL_STOP_COUNT;
		}
		if (stop_count--<=0){
			running = 0;
		}
	}
	motor_set_vel(MOTOR1, Wheel_V[0] , CLOSE_LOOP); 
	motor_set_vel(MOTOR2, Wheel_V[1] , CLOSE_LOOP);
	motor_set_vel(MOTOR3, Wheel_V[2] , CLOSE_LOOP);

}

int16_t distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
	return ((int16_t) sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

//Ref: http://www.intmath.com/plane-analytic-geometry/perpendicular-distance-point-line.php
//Get the perpendicular distance from the current point to the target line
int16_t get_ppdis(s16 curr_x,s16 curr_y){
	int32_t upperPart = track_m*curr_x - curr_y*100 + track_c;
	upperPart = upperPart < 0 ? -upperPart : upperPart;
	return upperPart/k_value;
}

//parallel speed
#define para_speed 1
void chase(){

	int16_t curr_x = get_X();
	int16_t curr_y = get_Y();																																																																																																			
	int16_t curr_angle = get_angle();
	uint32_t curr_ticks = get_full_ticks();
	y_index = 0;
	
	int16_t pp_dis = get_ppdis(curr_x, curr_y);
	if (curr_y*100<(track_m*curr_x + track_c)){
		//if on the right side of the target line, switch direction
		pp_dis = -pp_dis;
	}

	tft_clear();
	tft_prints(0, y_index++, "CX:%d CY:%d", curr_x, curr_y);
	tft_prints(0, y_index++, "TX:%d TY:%d", target_x, target_y);
	tft_prints(0, y_index++, "CBearing: %d", curr_angle);
	tft_prints(0, y_index++, "TBearing: %d", target_angle);
	tft_prints(0, y_index++, "PPdis: %d", pp_dis);

	uint16_t target_dis = distance(target_x, target_y, curr_x, curr_y);
	tft_prints(0, y_index++, "Distance: %d", target_dis);

	//perpendicular speed
	int16_t pp_v = pid(pp_dis);
	
	//Vector addition
	int16_t corr_bearing = int_arc_tan2(pp_v, para_speed) * 10;
	
	int16_t final_bearing = para_bearing + corr_bearing - curr_angle;
	final_bearing = normalise_angle(final_bearing);
	
	int16_t final_v = (int16_t) sqrt((pp_v*pp_v)+(para_speed*para_speed));
	
	int16_t tar_min_cur = target_angle-curr_angle;
	//Make sure tar_min_cur make the smallest turn
	tar_min_cur = normalise_angle(tar_min_cur);
	
	int16_t final_w = (tar_min_cur)/100;

	setRobotVelocity(final_bearing, final_v, final_w);
		
	tft_prints(0, y_index++, "%d %d %d", final_bearing, final_v, final_w);

	if (target_dis<50 || target_dis*target_dis<=pp_dis*pp_dis){
		stopping = 1;
		stop_count = TOTAL_STOP_COUNT;
	}

	tft_prints(0, y_index++, "%d %d %d", Wheel_V[0], Wheel_V[1], Wheel_V[2]);
	tft_prints(0, y_index++, "Time: %d", curr_ticks);
	tft_update();
}