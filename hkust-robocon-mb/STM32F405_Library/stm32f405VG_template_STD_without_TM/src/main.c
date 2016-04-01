#include "main.h"

int main(void) {
	SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
  tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
	
  RCC_ClocksTypeDef  rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	//Initialize timer variable  
	u32 ticks_ms_img = 0;
	uart_init(COM3,19200);
	while (1) {
		if(get_ticks() != ticks_ms_img){
			ticks_ms_img = get_ticks();
			tft_clear();
			printf("Test");

			tft_prints(0,0,"Count: %d",get_ticks());
			tft_prints(0,1,"Second: %d",get_seconds());
			
			tft_update();
		}
	}

}
