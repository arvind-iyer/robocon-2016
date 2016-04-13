#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;
bool imu_calc_init = false;
bool init_good = true;

int main(void) {
	led_init();
	TM_DELAY_Init();
	tft_easy_init(PIN_ON_BOTTOM); //Init LCD
	tft_put_logo(85, 120);
	buzzer_init();
	
	init_good |= mpu_init();
	
	Delayms(800);
	if (init_good){
		buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
	}else{
		buzzer_play_song(FAIL_SOUND, 100, 0);
	}
		
	while (1) {
		this_loop_ticks = get_ticks();
		if(last_loop_ticks != this_loop_ticks){
			any_loop_diff = this_loop_ticks - last_loop_ticks;
		
			if ((this_loop_ticks - last_long_loop_ticks) > LONG_LOOP_TICKS){
				any_loop_diff = this_loop_ticks - last_long_loop_ticks;
				if (!imu_calc_init){
					calc_init();
					//calibration_mode_loop();
					imu_calc_init = true;
				}
				led_blink(LED_D1);
				tft_clear();
				calcIMU();
				tft_println("%d %d", get_ticks(), any_loop_diff);
				tft_println("Yaw: %f", ypr[0]);
				tft_println("Pit: %f", ypr[1]);
				tft_println("Rol: %f", ypr[2]);
				tft_update();
				if (imu_ignoring_accel){
					led_control(LED_D2, LED_ON);
				}else{
					led_control(LED_D2, LED_OFF);
				}
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}
}
