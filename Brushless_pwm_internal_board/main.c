#include "main.h"
#include "LEDs.h"
#include "button.h"
#include "ticks.h"

#define max 1050
#define min 250
#define stepping 1

int main()
{
	u16 counter = 0;
	u16 duty_cycle = min;
	u32 myticks = 0;
	LED_INIT();
	tft_init(1, RED, BLACK, WHITE);
	button_init();
	servo_init();
	
	servo_control(1, min);
	tft_clear();
	tft_prints(0,0,"Now at %d", min);
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
	
	for (counter=min;counter<=max;counter = counter + stepping){
		servo_control(1, counter);
		tft_clear();
		tft_prints(0,0,"Now at %d", counter); 
		tft_update();
		//_delay_ms(3);
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
	
	for (counter=max;counter>=min;counter = counter - stepping){
		servo_control(1, counter);
		tft_clear();
		tft_prints(0,0,"Now at %d", counter);
		tft_update();
		//_delay_ms(3);
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
		_delay_ms(20);
		
		tft_clear();
		tft_prints(0,0,"Starting.");
		tft_update();
		_delay_ms(20);
		
		tft_clear();
		tft_prints(0,0,"Starting..");
		tft_update();
		_delay_ms(20);
		
		tft_clear();
		tft_prints(0,0,"Starting...");
		tft_update();
		_delay_ms(20);
	}
	
	tft_clear();
	tft_prints(0, 0, "%d", duty_cycle);
	tft_update();
	
	
	while(1){
		if (!read_button(1)){
			LED_ON(GPIOA, GPIO_Pin_15);
			if (duty_cycle<max){
				duty_cycle++;
			}
			tft_clear();
			tft_prints(0, 0, "%d", duty_cycle);
			tft_update();
			servo_control(1, duty_cycle);
			LED_OFF(GPIOA, GPIO_Pin_15);
		}
		if (!read_button(2)){
			LED_ON(GPIOB, GPIO_Pin_3);
			if (duty_cycle>min){
				duty_cycle--;
			}
			tft_clear();
			tft_prints(0, 0, "%d", duty_cycle);
			tft_update();
			servo_control(1, duty_cycle);
			LED_OFF(GPIOB, GPIO_Pin_3);
		}
	}
}