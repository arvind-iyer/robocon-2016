#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	SystemCoreClockUpdate();
	led_init();
	TM_DELAY_Init();
	imu_init();
	servo_init();
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	
	GAME_STAGE game_stage = SYSTEM_WAITING;
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			this_loop_ticks = get_ticks();
			
		}
	}
}
