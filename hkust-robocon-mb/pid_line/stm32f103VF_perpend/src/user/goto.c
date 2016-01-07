#include "goto.h"

int16_t target_x, target_y;
uint8_t running = 0;
int16_t V1, V2, V3;
int16_t track_m, track_c;
int16_t start_x, start_y;
int16_t para_bearing;

uint8_t isRunning(){
	return running == 1;
}

//Scaled by 100
#define Kp 15
#define Kd 600
int16_t lastEr = 0;
int16_t pid(int16_t errorDis){
	int16_t temp = (Kp*(errorDis) + Kd*(errorDis-lastEr))/100;
	lastEr = errorDis;
	if (errorDis<-50){
		if (errorDis<-200){
			if (errorDis<-500){
				return 50;
			}
			return -20;
		}
		return -5;
	}else if(errorDis>50){
		if (errorDis>200){
			if (errorDis>500){
				return 50;
			}
			return 20;
		}
		return 5;
	}else{
		return 0;
	}
//	if (errorDis<-20 || errorDis>20){
//		return temp;
//	}else{
//		return 0;
//	}
}

void setTargetPos(int16_t x, int16_t y){
	start_x = get_X();
	start_y = get_Y();
	int16_t curr_angle = get_angle();

	target_x = x;
	target_y = y;

	track_m = (target_y - start_y)*10/(target_x - start_x);
	track_c = start_x*10 - track_m * start_y;

	para_bearing = 900 - int_arc_tan2(target_y - start_y, target_x - start_x)*10 + curr_angle;

	running = 1;
}

void setRobotVelocity(int16_t angle, int16_t velocity) {
	V1 = int_sin(angle) * velocity * -1 / 10000;
	V2 = int_sin(angle+1200) * velocity * -1 / 10000;
	V3 = int_sin(angle+2400) * velocity * -1 / 10000;
	motor_set_vel(MOTOR1, V1 , CLOSE_LOOP); 
	motor_set_vel(MOTOR2, V2 , CLOSE_LOOP);
	motor_set_vel(MOTOR3, V3 , CLOSE_LOOP);
}

int16_t distance(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
	return ((int16_t) sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

void get_ppcor(s16 current_x,s16 current_y,s16* ppd_x,s16* ppd_y)
{
	*ppd_x = ((current_y - track_c)* track_m + current_x)/(1+track_m*track_m);
	*ppd_y = (track_m * *ppd_x + track_c)/10;
}


void easyStop() {
	running = 0;
	V1 = 0;
	V2 = 0;
	V3 = 0;
	motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
}

#define para_speed 1
void chase(){
	int16_t curr_x = get_X();
	int16_t curr_y = get_Y();																																																																																																			
	int16_t curr_angle = get_angle();
	uint32_t curr_ticks = get_full_ticks();
	int16_t pp_x, pp_y;

	get_ppcor(curr_x, curr_y, &pp_x, &pp_y);
	
	int16_t pp_dis = distance(curr_x, curr_y, pp_x, pp_y);
	if (curr_x>pp_x){
		pp_dis = -pp_dis;
	}

	tft_clear();
	uint8_t y_index = 0;
	tft_prints(0, y_index++, "CX:%d CY:%d", curr_x, curr_y);
	tft_prints(0, y_index++, "TX:%d TY:%d", target_x, target_y);
	tft_prints(0, y_index++, "CBearing: %d", curr_angle);
	tft_prints(0, y_index++, "PX:%d PY:%d", pp_x, pp_y);
	tft_prints(0, y_index++, "PDis: %d", pp_dis);

	uint16_t target_dis = distance(target_x, target_y, curr_x, curr_y);
	tft_prints(0, y_index++, "Distance: %d", target_dis);

	int16_t pp_v = pid(pp_dis);
	//pp_v = 1;
	
	tft_prints(0, y_index++, "PV: %d", pp_v);
	
	int16_t corr_bearing = int_arc_tan2(pp_v, para_speed) * 10;
	
	int16_t final_bearing = para_bearing + corr_bearing - curr_angle;
	setRobotVelocity(final_bearing, 
		(int16_t) sqrt((pp_v*pp_v)+(para_speed*para_speed)));
		
	tft_prints(0, y_index++, "Bearing:%d", final_bearing);

	if (target_dis<50 || target_dis*target_dis<=pp_dis*pp_dis){
		easyStop();
	}

	tft_prints(0, y_index++, "1:%d 2:%d 3:%d", V1, V2, V3);
	tft_prints(0, y_index++, "Time: %d", curr_ticks);
	tft_update();
}