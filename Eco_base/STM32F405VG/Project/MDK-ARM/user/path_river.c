#include "path_river.h"

u8 river_stage = 0;
u8 islands_count[2] = {0};
u8 last_IR_state[2] = {0};
float river_straight_yaw = 0;

void path_river_init(){
	#ifdef BLUE_FIELD
		river_straight_yaw = ardu_start_ypr[0] - 180.0f;
	#else
		river_straight_yaw = ardu_start_ypr[0] + 180.0f;
	#endif
	river_stage = 0;
	islands_count[0] = 0;
	islands_count[1] = 0;
	TM_GPIO_Init(IR_GPIO,IR_1_Pin|IR_2_Pin,TM_GPIO_Mode_IN,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_High); 
}

bool readIR(u8 id){
	//Invert the 2 IR when switch game field
	//Aftering inverting, id=0 is the first island it encounters
	#ifdef BLUE_FIELD
		if (id == 0){
			return GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
		}else{
			return GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
		}
	#else
		if (id == 1){
			return GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
		}else{
			return GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
		}
	#endif
}

void IR_update(){
	for (u8 i=0; i<2; i++){
		if (readIR(i)){
			if (last_IR_state[i] == 0){
				last_IR_state[i] = 1;
			}
		}else{
			//Only counts when IR signal is lost for a buffer time
			if (last_IR_state[i] > 0){
				last_IR_state[i]++;
				if (last_IR_state[i] >= IR_BUFFER_LENGTH){
					last_IR_state[i] = 0;
					islands_count[i]++;
				}
			}
		}
	}
}

GAME_STAGE path_river_update(){
	switch(river_stage){
		//case 0 first turn 90-degree
		case 0:
			if (fabs(river_straight_yaw - ardu_cal_ypr[0]) > 5.0f){
				#ifdef BLUE_FIELD
					force_set_angle(SERVO_MAX_PWM);
				#else
					force_set_angle(SERVO_MIN_PWM);
				#endif
			}else{
				river_stage++;
				enable_sensor_bar(RIVER_SENSOR_BAR_TRUST, RIVER_SENSOR_BAR_POWER, RIVER_SENSOR_BAR_Kp);
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw + 30.0f);
				#else
					set_target(river_straight_yaw - 30.0f);
				#endif
				targeting_update(ardu_cal_ypr[0]);
			}
			break;
			
		//case 1 follow white line til first island
		case 1:
			IR_update();
			//When it reaches the first island
			if (islands_count[0] >= 1){
				river_stage++;
				disable_sensor_bar();
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw);
				#else
					set_target(river_straight_yaw);
				#endif
			}
			
			//Track the white line with imu/sensor bar
			targeting_update(ardu_cal_ypr[0]);
			break;
			
		//case 2 go straight until it reaches the last island
		case 2:
			IR_update();
			//When it reaches the last island
			if (islands_count[1] >= 2){
				river_stage++;
				disable_sensor_bar();
			}
			targeting_update(ardu_cal_ypr[0]);
			break;
		case 3:
			return (GAME_STAGE) (CROSSING_RIVER + 1);
	}
	tft_println("RS:%d IR:%d %d", river_stage, readIR(0), readIR(1));
	tft_println("IC: %d %d", islands_count[0], islands_count[1]);
	tft_println("SY: %f", river_straight_yaw);
	return CROSSING_RIVER;
}
