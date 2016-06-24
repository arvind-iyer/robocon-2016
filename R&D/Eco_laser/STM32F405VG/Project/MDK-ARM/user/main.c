#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;
s32 last_ticks = 0;

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	ticks_init();
	led_init();
	#ifdef BLUE_FIELD
		tft_init((TFT_ORIENTATION)ORIENTATION_SETTING, BLACK, WHITE, BLUE); //Init LCD
	#else
		tft_init((TFT_ORIENTATION)ORIENTATION_SETTING, BLACK, WHITE, RED); //Init LCD
	#endif
	buzzer_init();
	button_init();
	tft_put_logo(85, 120);
	
	while(1){
		
	}
	
}
