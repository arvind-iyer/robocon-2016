#include "path_upslope_imu.h"

static u8 right_angle_ing = 0;
static u8 highland_count = 0; 
static u8 current_state = 1; // 0 = highland; 1 = slope
static s32 last_encoder_dis = SLOPE_SECTION_DIS;
static u16 upslope_servo_pwm = SERVO_MED_PWM;
static bool counting_encoder = false;
static u32 start_counting_encoder = 0;

void path_up_sb_init(u8 stage){
	right_angle_ing = 0;
	highland_count = stage;
	current_state = 0;
	last_encoder_dis = SLOPE_SECTION_DIS;
	counting_encoder = false;
}

GAME_STAGE path_up_sb_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (right_angle_ing == 1){
		if (abs(RIVER_STRAIGHT_YAW - mti_int_ypr[0]) > RIVER_90_MAX_TURN_UNTIL){
			si_clear();
			#ifdef BLUE_FIELD
				si_set_pwm(SERVO_PROPER_MAX_PWM);
			#else
				si_set_pwm(SERVO_PROPER_MIN_PWM);
			#endif
			si_execute();
		}else{
			return (GAME_STAGE)(CLIMBING_SLOPE + 1);
		}
	}else{
	
		if (flag == SENSOR_BAR_ALL){
			right_angle_ing = 1;
		}
		
		if (sensorbar_region == HIGH_ORANGE && current_state == 1 && (get_average_dis() - last_encoder_dis) >= SLOPE_SECTION_DIS){
			highland_count++;
			switch (highland_count){
				case 1:
					buzzer_play_song(HIGH_1, 500, 50);
					break;
				case 2:
					buzzer_play_song(HIGH_2, 300, 150);
					break;
				case 3:
					buzzer_play_song(HIGH_3, 250, 120);
					break;
			}
			current_state = 0;
			if (highland_count == 3){
				if (!counting_encoder){
					counting_encoder = true;
					start_counting_encoder = get_average_encoder();
				}
			}
		}else if(sensorbar_region == SLOPE_GREEN && current_state == 0){
		//}else if(sensorbar_region != 3 && current_state == 0){
			current_state = 1;
			last_encoder_dis = get_average_dis();
		}
		
		//u32 dynamic_river_dis = RIVER_90_TURN_ENC_Y/(RIVER_90_TURN_ENC_X+get_vel());
		u32 dynamic_river_dis = RIVER_90_TURN_ENC_CONSTANT;
		
		if (counting_encoder && (get_average_encoder() - start_counting_encoder)>dynamic_river_dis){
			right_angle_ing = 1;
		}
		
		if (flag != SENSOR_BAR_NTH){
			upslope_servo_pwm = sb_pwm_1to1(SLOPE_SB_INC_PWM, SLOPE_SB_DEC_PWM, 0);
			si_clear();
			si_set_pwm(upslope_servo_pwm);
			si_execute();
		}
		
		tft_println("SF: %d %d %d", highland_count, sensor_bar_mid, upslope_servo_pwm);
	}
	
	return CLIMBING_SLOPE;
}
