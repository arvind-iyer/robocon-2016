#include "main.h"

int main(void) {
	SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
  tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
	
	//Initialize the CAN protocol for motor
	//    can_init();
	//    can_rx_init();
	//    can_motor_init();
  RCC_ClocksTypeDef  rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	//Initialize timer variable  
	servo_init();
	u32 ticks_ms_img = 0;
	while (1) {
		if(get_ticks() != ticks_ms_img){
			ticks_ms_img = get_ticks();
			tft_clear();
			tft_prints(0,6, "Sys :%d", SystemCoreClock);
			tft_prints(0,0,"Sys:  %d",rcc_clocks.SYSCLK_Frequency);
			tft_prints(0,1,"HCLK: %d",rcc_clocks.HCLK_Frequency);
			tft_prints(0,2,"PCLK1:%d",rcc_clocks.PCLK1_Frequency);
			tft_prints(0,3,"PCLK2:%d",rcc_clocks.PCLK2_Frequency);
			tft_prints(0,4,"Count: %d",get_ticks());
			tft_prints(0,5,"Second: %d",get_seconds());
			if(get_seconds()>15)servo_control(SERVO1,4000);
			tft_update();
		}
	}

}
