#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	sensorbar_init();
	servo_init();
	tft_easy_init((TFT_ORIENTATION)ORIENTATION_SETTING); //Init LCD
	buzzer_init();
	button_init();
	tft_put_logo(85, 120);
	force_set_angle(SERVO_MED_DEG);
	
	bool cali_done = false;
	u16 servo_val = SERVO_MED_DEG;
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			this_loop_ticks = get_ticks();
			
			//Long loop action
			any_loop_diff = this_loop_ticks - last_long_loop_ticks;
			if (any_loop_diff > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				tft_clear();
				tft_println("[~GREY KNIGHT~]");
				
				if (button_pressed(BUTTON_JS_CENTRE) && !cali_done){
					cali_done = true;
					buzzer_play_song(MARIO_BEGIN, 50, 0);
				}
				
				if (button_pressed(BUTTON_JS_UP)){
					servo_val ++;
				}
				
				if (button_pressed(BUTTON_JS_DOWN)){
					servo_val --;
				}
				
				force_set_angle(servo_val);
				tft_println("%d", servo_val);
				
				if (cali_done){
					sensor_bar_track();
				}
			
				last_long_loop_ticks = this_loop_ticks;
				tft_update();
			}
			last_loop_ticks = this_loop_ticks;
		}
	}
}
