#include "path_river.h"

u8 river_stage = 0;
u8 islands_count[2] = {0};
u8 last_IR_state[2] = {0};
s16 river_straight_yaw = 0;

void path_river_init(){
	#ifdef BLUE_FIELD
		river_straight_yaw = ardu_int_ypr[0] - 1800;
	#else
		river_straight_yaw = ardu_int_ypr[0] + 1800;
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
			if (abs(river_straight_yaw - ardu_int_ypr[0]) > 50){
				si_clear();
				#ifdef BLUE_FIELD
					si_set_pwm(SERVO_MAX_PWM);
				#else
					si_set_pwm(SERVO_MIN_PWM);
				#endif
				si_execute();
			}else{
				river_stage++;
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw + 300);
				#else
					set_target(river_straight_yaw - 300);
				#endif
			}
			break;
			
		//case 1 follow white line til first island
		case 1:
			IR_update();
			//When it reaches the first island
			if (islands_count[0] >= 1){
				river_stage++;
				#ifdef BLUE_FIELD
					set_target(river_straight_yaw);
				#else
					set_target(river_straight_yaw);
				#endif
			}
			
			si_clear();
			//Track the white line with imu/sensor bar
			si_add_pwm_bias(sensor_bar_get_corr_nf(RIVER_SENSOR_BAR_POWER, RIVER_SENSOR_BAR_Kp));
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
			
		//case 2 go straight until it reaches the last island
		case 2:
			IR_update();
			//When it reaches the last island
			if (islands_count[1] >= 2){
				river_stage++;
			}
			si_clear();
			targeting_update(ardu_int_ypr[0]);
			si_execute();
			break;
		case 3:
			return (GAME_STAGE) (CROSSING_RIVER + 1);
	}
	tft_println("RS:%d IR:%d %d", river_stage, readIR(0), readIR(1));
	tft_println("IC: %d %d", islands_count[0], islands_count[1]);
	tft_println("SY: %d", river_straight_yaw);
	return CROSSING_RIVER;
}