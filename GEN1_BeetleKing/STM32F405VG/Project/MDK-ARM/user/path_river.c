#include "path_river.h"

u8 river_stage = 0;
float river_straight_yaw;
u8 islands_count[2] = {0};
u8 last_IR_state[2] = {0};

void path_river_init(){
	river_straight_yaw = start_ypr[0] - 180.0f;
	river_stage = 0;
	islands_count[0] = 0;
	islands_count[1] = 0;
	TM_GPIO_Init(IR_GPIO,IR_1_Pin|IR_2_Pin,TM_GPIO_Mode_IN,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_High); 
}

bool readIR(u8 id){
	if (id == 0){
		return GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
	}else{
		return GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
	}
}

GAME_STAGE path_river_update(){
	switch(river_stage){
		case 0:
			if (abs_diff(river_straight_yaw, cal_ypr[0]) < -5.0f){
				force_set_angle(SERVO_MED_DEG + 60.0f);
			}else{
				river_stage++;
				set_target(river_straight_yaw);
				targeting_update(cal_ypr[0]);
			}
			break;
		case 1:
			for (u8 i=0; i<2; i++){
				if (readIR(i)){
					if (last_IR_state[i] == 0){
						last_IR_state[i] = 1;
					}
					set_target(river_straight_yaw + readIR(0)*-40 + readIR(1)*40);
				}else{
					//Only counts when IR signal is lost for a buffer time
					if (last_IR_state[i] > 0){
						last_IR_state[i]++;
						if (last_IR_state[i] >= IR_BUFFER_LENGTH){
							last_IR_state[i] = 0;
							islands_count[i]++;
							if (islands_count[1] >= 2){
								river_stage++;
								set_target(river_straight_yaw + 45);
							}
						}
					}
				}
			}
			targeting_update(cal_ypr[0]);
			break;
		case 2:
			set_target(river_straight_yaw + 45);
			targeting_update(cal_ypr[0]);
			break;
	}
	tft_println("RS:%d IR:%d %d", river_stage, readIR(0), readIR(1));
	tft_println("IC: %d %d", islands_count[0], islands_count[1]);
	tft_println("SY: %f", river_straight_yaw);
	return CROSSING_RIVER;
}