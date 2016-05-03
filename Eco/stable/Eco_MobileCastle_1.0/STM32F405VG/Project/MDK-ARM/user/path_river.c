#include "path_river.h"

u8 river_stage = 0;
u8 islands_count[2] = {0};
u8 last_IR_state[2] = {0};
s16 river_straight_yaw = 0;

void path_river_init(s16 straight_yaw){
	river_straight_yaw = straight_yaw;
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

void IR_update(u8 id){
	if (readIR(id)){
		if (last_IR_state[id] == 0){
			last_IR_state[id] = 1;
		}
	}else{
		//Only counts when IR signal is lost for a buffer time
		if (last_IR_state[id] > 0){
			last_IR_state[id]++;
			if (last_IR_state[id] >= IR_BUFFER_LENGTH){
				last_IR_state[id] = 0;
				islands_count[id]++;
			}
		}
	}
}

GAME_STAGE path_river_update(){
	switch(river_stage){
		case 0:
			river_stage++;
			#ifdef BLUE_FIELD
				set_target(river_straight_yaw + 35);
			#else
				set_target(river_straight_yaw - 35);
			#endif
			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
			break;
		
		//Keep going until first island
		case 1:
			IR_update(0);
			//When it reaches the first island
			if (islands_count[0] >= 1){
				river_stage++;
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw - 15);
				#else
					set_target(river_straight_yaw + 15);
				#endif
				buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
			}
			
			si_clear();
			//Track the white line with imu/sensor bar
			//si_add_pwm_bias(sensor_bar_get_corr_nf(RIVER_SENSOR_BAR_POWER, RIVER_SENSOR_BAR_Kp)*10);
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
			
		//case 2 go straight until it reaches the last island
		case 2:
			IR_update(1);
			//When it reaches the last island
			if (islands_count[1] >= 2){
				river_stage++;
				buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
			}
			si_clear();
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
		case 3:
			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
			return (GAME_STAGE) (CROSSING_RIVER + 1);
	}
	tft_println("RS:%d IR:%d %d", river_stage, readIR(0), readIR(1));
	tft_println("IC: %d %d", islands_count[0], islands_count[1]);
	tft_println("SY: %d", river_straight_yaw);
	return CROSSING_RIVER;
}
