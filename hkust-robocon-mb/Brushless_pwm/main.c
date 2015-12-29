#include "main.h"
#include "LEDs.h"
#include "button.h"
#include "ticks.h"

int main()
{
	u8 duty_cycle = 50;
	LED_INIT();
	motor_init();
	tft_init(1, RED, BLACK, WHITE);
	motor_control(1, 1, 166*duty_cycle/100);
	button_init();
	while(1){
		if (!read_button(1)){
			LED_ON(GPIOA, GPIO_Pin_15);
			if (duty_cycle<100){
				duty_cycle++;
			}
			tft_clear();
			tft_prints(0, 0, "%d", duty_cycle);
			tft_update();
			motor_control(1, 1, 166*duty_cycle/100);
			LED_OFF(GPIOA, GPIO_Pin_15);
		}
		if (!read_button(2)){
			LED_ON(GPIOB, GPIO_Pin_3);
			if (duty_cycle>0){
				duty_cycle--;
			}
			tft_clear();
			tft_prints(0, 0, "%d", duty_cycle);
			tft_update();
			motor_control(1, 1, 166*duty_cycle/100);
			LED_OFF(GPIOB, GPIO_Pin_3);
		}
		_delay_ms(10);
	}
}