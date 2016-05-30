#include "path_upslope_imu.h"

static u8 right_angle_ing = 0;
static u8 highland_count = 0; 
static u8 current_state = 1; // 0 = highland; 1 = slope
static u32 last_encoder_val = 0;
static u16 upslope_servo_pwm = SERVO_MED_PWM;
static bool counting_encoder = false;
static u32 start_counting_encoder = 0;

void path_up_sb_init(u8 stage){
	right_angle_ing = 0;
	highland_count = stage;
	current_state = 0;
	last_encoder_val = 0;
	counting_encoder = false;
}

GAME_STAGE path_up_sb_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (right_angle_ing == 1){
		if (abs(river_straight_yaw - ardu_int_ypr[0]) > RIVER_90_MAX_TURN_UNTIL){
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
		
		if (sensorbar_region == HIGH_ORANGE && current_state == 1 && (get_average_encoder() - last_encoder_val) > SLOPE_SECTION_ENC){
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
			last_encoder_val = get_average_encoder();
		}
		
		if (counting_encoder && (get_average_encoder() - start_counting_encoder)>RIVER_90_TURN_ENC_CONSTANT){
			right_angle_ing = 1;
		}
		
		if (flag != SENSOR_BAR_NTH){
			upslope_servo_pwm = sb_pwm_1to1(SLOPE_SB_INC_PWM, SLOPE_SB_DEC_PWM);
			si_clear();
			si_set_pwm(upslope_servo_pwm);
			si_execute();
		}
		
		tft_println("SF: %d %d %d", highland_count, sensor_bar_mid, upslope_servo_pwm);
	}
	
	return CLIMBING_SLOPE;
}