#include "path_river.h"

static u8 river_stage = 0;
static u8 islands_start_count[2] = {0};
static u8 islands_end_count[2] = {0};
static bool last_IR_read[2] = {0};
static s16 IR_buffer[2] = {0};
static bool IR_buffered_read[2] = {0};
static s32 start_dis = 0;

void path_river_init(s16 straight_yaw){
	river_stage = 0;
	for (u8 i=0;i<2;i++){
		IR_buffered_read[i] = false;
		islands_start_count[i] = 0;
		islands_end_count[i] = 0;
		last_IR_read[i] = false;
	}
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

s32 last_IR_end_enc[2] = {0};
void IR_island_update(u8 id){
	IR_buffer_update(id);
	if (IR_buffered_read[id]){
		if (!last_IR_read[id] && (get_average_dis()-last_IR_end_enc[id])>IR_ENCODER_DIS){
		//if (!last_IR_read[id]){
			islands_start_count[id]++;
			last_IR_read[id] = true;
		}
	}else{
		if (last_IR_read[id]){
			islands_end_count[id]++;
			last_IR_end_enc[id] = get_average_dis();
			last_IR_read[id] = false;
		}
	}
}

GAME_STAGE path_river_update(){
	switch(river_stage){
		case 0:
			river_stage++;
			#ifdef BLUE_FIELD
				set_target(RIVER_STRAIGHT_YAW + 35);
			#else
				set_target(RIVER_STRAIGHT_YAW - 35);
			#endif
			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
			break;
		
		//Keep going until first island
		case 1:
			IR_island_update(FIRST_IR_ID);
			SENSOR_BAR_FLAG flag;
	
			sensor_bar_get_corr(1, 100, &flag);
			
			if (flag != SENSOR_BAR_NTH){
				#ifdef BLUE_FIELD
					u16 river_servo_pwm = sb_pwm_1to1(RIVER_SB_INC_PWM, RIVER_SB_DEC_PWM, -RIVER_FIRST_SENSOR_BAR_BIAS);
				#else
					u16 river_servo_pwm = sb_pwm_1to1(RIVER_SB_INC_PWM, RIVER_SB_DEC_PWM, RIVER_FIRST_SENSOR_BAR_BIAS);
				#endif
				
				si_clear();
				si_set_pwm(river_servo_pwm);
				si_execute();
			}
		
			//When it reaches the first island
			if (islands_start_count[FIRST_IR_ID] >= 1){
				river_stage++;
				#ifdef BLUE_FIELD
					set_target(RIVER_STRAIGHT_YAW);
				#else
					set_target(RIVER_STRAIGHT_YAW);
				#endif
				buzzer_play_song(RIVER_1, 120, 30);
			}
			break;
			
		//case 2 go straight until it reaches thrid island
		case 2:
			IR_island_update(0);
			IR_island_update(1);
			//When it reaches the last island
			if (islands_end_count[FIRST_IR_ID] >= 2){
				river_stage++;
				start_dis = get_average_dis();
				buzzer_play_song(RIVER_2, 200, 30);
				set_target(RIVER_STRAIGHT_YAW + TURNING_AFTER_SECOND_ISLAND);
			}
			si_clear();
			targeting_update(mti_int_ypr[0]);
			si_execute();
			break;
			
		//case 3 keep going until encoder buffer ended or senses the last island
		case 3:
			#ifdef BLUE_FIELD
				if ((get_average_dis() - start_dis > DIS_AFTER_THIRD_ISLAND) || islands_end_count[SECOND_IR_ID] >= 2){
			#else
				if ((get_average_dis() - start_dis > DIS_AFTER_THIRD_ISLAND) || islands_end_count[SECOND_IR_ID] >= 2){
			#endif
				river_stage++;
			}
			si_clear();
			targeting_update(mti_int_ypr[0]);
			si_execute();
			break;
			
		case 4:
			buzzer_play_song(HIGH_4, 200, 50);
			path_down_reset();
			return (GAME_STAGE) (CROSSING_RIVER + 1);
	}
	tft_println("IR:%d %d %d %d", readIR(FIRST_IR_ID), readIR(SECOND_IR_ID), get_ir_dis(FIRST_IR_ID), get_ir_dis(SECOND_IR_ID));
	tft_println("IC: %d %d", islands_start_count[0], islands_start_count[1], islands_end_count[0], islands_end_count[1]);
	return CROSSING_RIVER;
}
