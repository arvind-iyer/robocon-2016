#include "track.h"

uint32_t track_length[track_segment_number] = 
	{2748, 1400, 1850, 1950, 3000, 1150, 1560, 1400, 2054, 2079};
uint16_t track_slope[track_segment_number] = 
	{0};
uint16_t track_servo_val[track_segment_number] = 
	{1550, 2200, 1498, 1027, 1329, 2148, 1990, 1650, 1118, 1537};

u8 current_track_index = 0;
u32 curr_track_length = 0;
u16 curr_track_slope = 0;
u16 curr_track_servo = 0;

u16 servo_val = SERVO_MED;

u8 free_drive = 0;

void update_curr_track(){
	curr_track_length = track_length[current_track_index];
	curr_track_servo = track_servo_val[current_track_index];
	curr_track_slope = track_slope[current_track_index];
	servo_val = curr_track_servo;
	
	if (curr_track_length == 0){
		free_drive = 1;
	}
}

void track_init(){
	u8 index = 0;

	servo_val = SERVO_MED;
	current_track_index = 0;
	update_curr_track();
	free_drive = 0;
}

void track_update(){
	encoder_update();
	
	if (free_drive == 1){
		
		if (button_pressed(BUTTON_JS_UP)){
			servo_val++;
		}else if(button_pressed(BUTTON_JS_DOWN)){
			servo_val--;
		}

	}else if ((get_count(ENCODER1)+get_count(ENCODER2))/2 > curr_track_length){
		encoder_reset_reading();
		current_track_index++;
		update_curr_track();
	}
	
	servo_control(SERVO1, servo_val);
	servo_control(SERVO2, servo_val);
	servo_control(SERVO3, servo_val);
	servo_control(SERVO4, servo_val);

	tft_append_line("%d", servo_val);
	tft_append_line("%d", current_track_index);
}

u16 last_test_ticks = 0;
void track_test(){
	tft_clear();
	while(1){
		if (last_test_ticks==get_ticks()) continue;
		last_test_ticks = get_ticks();
		button_update();
		encoder_update();
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}

		if (button_pressed(BUTTON_1)){
			while(button_pressed(BUTTON_1)){
				button_update();
			}
			tft_append_line("%d %d", (get_count(ENCODER1)+get_count(ENCODER2))/2, servo_val);
			tft_update();
			encoder_reset_reading();
		}

		if (button_pressed(BUTTON_JS_UP)){
			servo_val++;
		}else if(button_pressed(BUTTON_JS_DOWN)){
			servo_val--;
		}
		
		servo_control(SERVO1, servo_val);
		servo_control(SERVO2, servo_val);
		servo_control(SERVO3, servo_val);
		servo_control(SERVO4, servo_val);
		
	}
}