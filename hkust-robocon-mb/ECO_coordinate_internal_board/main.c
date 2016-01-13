#include "main.h"

int main(void) {
	
	LED_INIT();
	
	tft_init(3, WHITE, BLACK, RED);
	ticks_init();
	encoder_init();
	
	while(1){

		LED_ON(GPIOA, GPIO_Pin_15);
 
		tft_clear();

		tft_prints(0, 0, "%d", getCount());
		tft_prints(0, 1, "%d", 0);
		tft_update();
		
		LED_OFF(GPIOA, GPIO_Pin_15);
	}
}