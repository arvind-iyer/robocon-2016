#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	ticks_init();
	led_init();
	servo_init();
	tft_easy_init((TFT_ORIENTATION)ORIENTATION_SETTING); //Init LCD
	button_init();
	buzzer_init();
	tft_put_logo(85, 120);
	
	GAME_STAGE game_stage = SYSTEM_WAITING;
	
	while (1) {
			tft_clear();
			servo_control(SERVO1, 1500);
			tft_println("%d", 1500);
			tft_println("%d", get_full_ticks());
			tft_update();
//		for (u16 i=1000;i<3000;i=i+2){
//			tft_clear();
//			servo_control(SERVO1, i);
//			tft_println("%d", i);
//			tft_println("%d", get_full_ticks());
//			tft_update();
//		}
//		for (u16 i=3000;i>1000;i=i-2){
//			tft_clear();
//			servo_control(SERVO1, i);
//			tft_println("%d", i);
//			tft_println("%d", get_full_ticks());
//			tft_update();
//		}
	}
}
