#include "path_river.h"

static u8 river_stage = 0;
static u8 islands_start_count[2] = {0};
static u8 islands_end_count[2] = {0};
static bool last_IR_read[2] = {0};
static s16 IR_buffer[2] = {0};
static bool IR_buffered_read[2] = {0};
s16 river_straight_yaw = 0;
static u32 encoder_start_reading = 0;

void path_river_init(s16 straight_yaw){
	river_straight_yaw = straight_yaw;
	river_stage = 0;
	for (u8 i=0;i<2;i++){
		IR_buffered_read[i] = false;
		islands_start_count[i] = 0;
		islands_end_count[i] = 0;
		last_IR_read[i] = false;
	}
	TM_GPIO_Init(IR_GPIO,IR_1_Pin|IR_2_Pin,TM_GPIO_Mode_IN,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_High); 
}

bool readIR(u8 id){
	//Invert the 2 IR when switch game field
	//Aftering inverting, id=0 is the first island it encounters
	#ifdef BLUE_FIELD
		if (id == 0){
			return !GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
		}else{
			return !GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
		}
	#else
		if (id == 1){
			return !GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
		}else{
			return !GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
		}
	#endif
}

void IR_buffer_update(u8 i){
	if (readIR(i)){
		if (IR_buffer[i]<0){
			IR_buffer[i] = 0;
		}else{
			IR_buffer[i]++;
			if (abs(IR_buffer[i]) > IR_BUFFER_LENGTH){
				IR_buffered_read[i] = true;
			}
		}
	}else{
		if (IR_buffer[i]>0){
			IR_buffer[i] = 0;
		}else{
			IR_buffer[i]--;
			if (abs(IR_buffer[i]) > IR_BUFFER_LENGTH){
				IR_buffered_read[i] = false;
			}
		}
	}
}

void IR_island_update(u8 id){
	IR_buffer_update(id);
	if (IR_buffered_read[id]){
		if (!last_IR_read[id]){
			islands_start_count[id]++;
			last_IR_read[id] = true;
		}
	}else{
		if (last_IR_read[id]){
			islands_end_count[id]++;
			last_IR_read[id] = false;
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
			IR_island_update(0);
			SENSOR_BAR_FLAG flag;
	
			sensor_bar_get_corr(1, 100, &flag);
			
			if (flag != SENSOR_BAR_NTH){
				u16 river_servo_pwm = sb_pwm_1to1(RIVER_SB_INC_PWM, RIVER_SB_DEC_PWM);
				
				si_clear();
				si_set_pwm(river_servo_pwm);
				si_execute();
			}
		
			//When it reaches the first island
			if (islands_start_count[0] >= 1){
				river_stage++;
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw);
				#else
					set_target(river_straight_yaw);
				#endif
				buzzer_play_song(RIVER_1, 120, 30);
			}
			break;
			
					//case 2 go straight until it reaches the last island
		case 2:
			IR_island_update(0);
			IR_island_update(1);
			//When it reaches the last island
			if (islands_end_count[0] >= 2){
				river_stage++;
				encoder_start_reading = get_average_encoder();
				buzzer_play_song(RIVER_2, 200, 30);
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw + 30);
				#else
					set_target(river_straight_yaw - 30);
				#endif
			}
			si_clear();
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
			
		case 3:
			#ifdef BLUE_FIELD
				if (get_average_encoder()-encoder_start_reading > ENCODER_AFTER_THIRD_ISLAND){
			#else
				if (get_average_encoder()-encoder_start_reading > ENCODER_AFTER_THIRD_ISLAND){
			#endif
				river_stage++;
			}
			si_clear();
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
			
		case 4:
			buzzer_play_song(HIGH_4, 200, 50);
			path_down_reset();
			return (GAME_STAGE) (CROSSING_RIVER + 1);
	}
	tft_println("IR:%d %d %d %d", readIR(0), readIR(1), last_IR_read[0], last_IR_read[1]);
	tft_println("IC: %d %d", islands_start_count[0], islands_start_count[1], islands_end_count[0], islands_end_count[1]);
	return CROSSING_RIVER;
}
