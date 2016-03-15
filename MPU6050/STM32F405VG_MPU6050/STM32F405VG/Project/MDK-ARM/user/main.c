#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	mpu_init();
	buzzer_init();
	
	mpu_wake_up();
		
	while (1) {
		this_loop_ticks = get_ticks();
		if(last_loop_ticks != this_loop_ticks){
			getRawAccelGyro(IMU_Buffer);   
			tft_clear();
			tft_println("%d", get_ticks());
			for (u8 i=0;i<6;i++){
				tft_println("%d", IMU_Buffer[i]);
			}
			tft_update();
		
			if ((this_loop_ticks - last_long_loop_ticks) > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}
}
