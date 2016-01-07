#include "goto.h"

int16_t target_x, target_y;
int16_t carrot_x, carrot_y;
int16_t step_x, step_y;
uint16_t steps_remain;
uint8_t running = 0;
int16_t V1, V2, V3;

uint8_t isRunning(){
	return running == 1;
}

//Scaled by 100
#define Kp 20
#define Kd 80
int16_t lastEr = 0;
int16_t pid(int16_t errorDis){
	int16_t temp = (Kp*(errorDis) + Kd*(errorDis-lastEr))/100;
	lastEr = errorDis;
	if (temp<0 && temp>-20){
		temp = -20;
	}else if(temp>0 && temp<20){
		temp = 20;
	}
	return temp;
}

void setTargetPos(int16_t x, int16_t y, int16_t steps){
	int16_t curr_x = get_X();
	int16_t curr_y = get_Y();

	target_x = x;
	target_y = y;

	step_x = (target_x - curr_x) / steps;
	step_y = (target_y - curr_y) / steps;

	carrot_x = curr_x + step_x;
	carrot_y = curr_y + step_y;

	steps_remain = steps;

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

void easyStop() {
	running = 0;
	V1 = 0;
	V2 = 0;
	V3 = 0;
	motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
}

#define carrot_const 0
void chase(){
	int16_t curr_x = get_X();
	int16_t curr_y = get_Y();																																																																																																			
	int16_t curr_angle = get_angle();
	uint32_t curr_ticks = get_full_ticks();

	int16_t carrot_dis = distance(carrot_x, carrot_y, curr_x, curr_y);
	tft_clear();
	uint8_t y_index = 0;
	tft_prints(0, y_index++, "CX:%d CY:%d", curr_x, curr_y);
	tft_prints(0, y_index++, "CBearing: %d", curr_angle);

	//Give next carrot
	if (distance(carrot_x-step_x, carrot_y-step_y, curr_x, curr_y) > carrot_dis && steps_remain>0){
		carrot_x += step_x;
		carrot_y += step_y;
		steps_remain--;
	}

	//If enter carrot mode
	if (carrot_dis>carrot_const){
		int16_t carrot_angle = (900 - int_arc_tan2(carrot_y - curr_y, carrot_x - curr_x)*10 + curr_angle);
		setRobotVelocity(carrot_angle, pid(carrot_dis));
		tft_prints(0, y_index++, "CARRROT!");
		tft_prints(0, y_index++, "AX:%d AY:%d", carrot_x, carrot_y);
		tft_prints(0, y_index++, "TBearing: %d", carrot_angle);
		tft_prints(0, y_index++, "C Dis: %d", carrot_dis);
	}else{
		int16_t target_angle = (900 - int_arc_tan2(target_y - curr_y, target_x - curr_x)*10 + curr_angle);
		setRobotVelocity(target_angle, 50);
		tft_prints(0, y_index++, "No carrot for you");
		tft_prints(0, y_index++, "TX:%d TY:%d", target_x, target_y);
		tft_prints(0, y_index++, "TBearing: %d", target_angle);
	}

	uint16_t target_dis = distance(target_x, target_y, curr_x, curr_y);
	tft_prints(0, y_index++, "Distance: %d", target_dis);
	if (target_dis<70){
		easyStop();
	}

	tft_prints(0, y_index++, "Steps:%d", steps_remain);
	tft_prints(0, y_index++, "1:%d 2:%d 3:%d", V1, V2, V3);
	tft_prints(0, y_index++, "Time: %d", curr_ticks);
	tft_update();
}