#include "main.h"
#include "LEDs.h"
#include "button.h"
#include "ticks.h"

int main()
{
	u16 counter = 0;
	u8 duty_cycle = 30;
	u32 myticks = 0;
	LED_INIT();
	tft_init(1, RED, BLACK, WHITE);
	button_init();
	servo_init();
	
	servo_control(1, 9250);
	tft_clear();
	tft_prints(0,0,"Now at 0 degree");
	tft_update();
	
	while(read_button(0));
	if (!read_button(0)){
		LED_ON(GPIOB, GPIO_Pin_4);
		tft_clear();
		tft_prints(0,0,"End first phase");
		tft_update();
		while(!read_button(0));
		tft_clear();
		tft_update();
		LED_OFF(GPIOB, GPIO_Pin_4);
	}
	
	for (counter=9250;counter>=8950;counter--){
		servo_control(1, counter);
		tft_clear();
		tft_prints(0,0,"Now at %d", counter);
		tft_update();
		_delay_ms(3);
	}
	
	while(read_button(0));
	if (!read_button(0)){
		LED_ON(GPIOB, GPIO_Pin_4);
		tft_clear();
		tft_prints(0,0,"End second phase");
		tft_update();
		while(!read_button(0));
		LED_OFF(GPIOB, GPIO_Pin_4);
	}
	
	for (counter=8950;counter<=9250;counter++){
		servo_control(1, counter);
		tft_clear();
		tft_prints(0,0,"Now at %d", counter);
		tft_update();
		_delay_ms(3);
	}
	
	while(read_button(0));
	if (!read_button(0)){
		LED_ON(GPIOB, GPIO_Pin_4);
		tft_clear();
		tft_prints(0,0,"Starting...");
		tft_update();
		while(!read_button(0));
		LED_OFF(GPIOB, GPIO_Pin_4);
	}
	
	for (counter=0;counter<5;counter++){
		tft_clear();
		tft_prints(0,0,"Starting");
		tft_update();
		_delay_ms(100);
		
		tft_clear();
		tft_prints(0,0,"Starting.");
		tft_update();
		_delay_ms(100);
		
		tft_clear();
		tft_prints(0,0,"Starting..");
		tft_update();
		_delay_ms(100);
		
		tft_clear();
		tft_prints(0,0,"Starting...");
		tft_update();
		_delay_ms(100);
	}
	
	tft_clear();
	tft_prints(0, 0, "%d", duty_cycle);
	tft_update();
	
	while(1){
		if (!read_button(1)){
			LED_ON(GPIOA, GPIO_Pin_15);
			if (duty_cycle<100){
				duty_cycle++;
			}
			tft_clear();
			tft_prints(0, 0, "%d", duty_cycle);
			tft_update();
			servo_control(1, 10000*(100-duty_cycle)/100);
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
			servo_control(1, 10000*(100-duty_cycle)/100);
			LED_OFF(GPIOB, GPIO_Pin_3);
		}
		_delay_ms(100);
	}
}