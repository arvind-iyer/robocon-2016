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
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	buzzer_init();
	
	init_good |= mpu_init();
	
	Delayms(500);
	if (init_good){
		buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
	}else{
		buzzer_play_song(FAIL_SOUND, 100, 0);
	}
		
	while (1) {
		this_loop_ticks = get_ticks();
		if(last_loop_ticks != this_loop_ticks){
			any_loop_diff = this_loop_ticks - last_loop_ticks;
			getRawAccelGyro();  
			if (!imu_calc_init){
				calc_init();
				imu_calc_init = true;
			}
		
			if ((this_loop_ticks - last_long_loop_ticks) > LONG_LOOP_TICKS){
				any_loop_diff = this_loop_ticks - last_long_loop_ticks;
				led_blink(LED_D1);
				tft_clear();
				calcIMU();
				tft_println("%d %d", get_ticks(), any_loop_diff);
				tft_println("Rx: %f", Rest[0]);
				tft_println("Ry: %f", Rest[1]);
				tft_println("Rz: %f", Rest[2]);
				tft_println("Yaw: %f", ypr[0]);
				tft_println("Pit: %f", ypr[1]);
				tft_println("Rol: %f", ypr[2]);
				for (u8 i=0;i<3;i++){
					//tft_println("%d %d", IMU_Buffer[i*2], IMU_Buffer[i*2+1]);
				}
				tft_update();
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}
}
