#include "main.h"
#include "LEDs.h"
#include "button.h"
#include "ticks.h"

#define max 1050
#define min 400
#define stepping 1

int main(){
	u16 duty_cycle = min;
	LED_INIT();
	tft_init(1, WHITE, BLACK, RED);
	u8 self_checked = 0;
	button_init();
	servo_init();
	ticks_init();
	
	servo_control(1, min);
	
	u8 self_check_counter = 0;

	
	while(self_checked==0){
		LED_ON(GPIOB, GPIO_Pin_4);
		
		if (get_ms_ticks()%200==0){
		
			tft_clear();
			switch (self_check_counter){
				case 0:
					tft_prints(0,0,"Self Checking");
					tft_prints(0, 3, "    / \\  ");
					break; 
				case 1:
					tft_prints(0,0,"Self Checking.");
					tft_prints(0, 3, "    / |   ");
					break;
				case 2:
					tft_prints(0,0,"Self Checking..");
					tft_prints(0, 3, "    | |  ");
					break;
				case 3:
					tft_prints(0,0,"Self Checking...");
					tft_prints(0, 3, "    / |   ");
					break;
				case 4:
					tft_prints(0,0,"Self Checking....");
					tft_prints(0, 3, "    / \\  ");
					break;
				case 5:
					tft_prints(0,0,"Self Checking.....");
					tft_prints(0, 3, "    | \\  ");
					break;
			}
			
			switch(self_check_counter){
				case 0:
					tft_prints(0, 2, " ~~(o_o)~-");
					break;
				case 1:
					tft_prints(0, 2, " ~~(o_o)--");
					break;
				case 2:
					tft_prints(0, 2, " ~-(o_o)~~");
					break;
				case 3:
					tft_prints(0, 2, " --(o_o)~~");
					break;
				default:
					tft_prints(0, 2, " ~~(o_o)~~");
					break;
			}
			
			tft_prints(0,5,"Any button to start");
			
			self_check_counter = (self_check_counter+1)%5;
			tft_update();
		}
		
		if (!read_button(0) || !read_button(1) || !read_button(2)){
			while(!read_button(0) || !read_button(1) || !read_button(2));
			tft_clear();
			tft_prints(0,0,"Starting...");
			tft_update();
			self_checked = 1;
		}
		
		LED_OFF(GPIOB, GPIO_Pin_4);
	}

	u8 display_val[3] = {9};
	tft_clear();
	tft_update();
	
	tft_put_mega_ass_num(0, 1, 0, BLACK);
	tft_put_mega_ass_num(10, 1, 0, BLACK);
	tft_put_mega_ass_num(20, 1, 0, BLACK);
	tft_mega_update();
	
	while(1){
		if (!read_button(1)){
			LED_ON(GPIOA, GPIO_Pin_15);
			if (duty_cycle<max){
				duty_cycle += stepping;
			}
			servo_control(1, duty_cycle);
			LED_OFF(GPIOA, GPIO_Pin_15);
		}
		
		if (!read_button(2)){
			LED_ON(GPIOB, GPIO_Pin_3);
			if (duty_cycle>min){
				duty_cycle -= stepping;
			}
			servo_control(1, duty_cycle);
			LED_OFF(GPIOB, GPIO_Pin_3);
		}
	
		u16 mapped_val = (duty_cycle-min)*999/(max-min);
		
		if (display_val[0] != mapped_val/100){
			tft_put_mega_ass_num(0, 1, display_val[0], WHITE);
			display_val[0] = mapped_val/100;
			tft_put_mega_ass_num(0, 1, display_val[0], BLACK);
		}
		
		if (display_val[1] != (mapped_val/10)%10){
			tft_put_mega_ass_num(10, 1, display_val[1], WHITE);
			display_val[1] = (mapped_val/10)%10;
			tft_put_mega_ass_num(10, 1, display_val[1], BLACK);
		}
				
		if (display_val[2] != mapped_val%10){
			tft_put_mega_ass_num(20, 1, display_val[2], WHITE);
			display_val[2] = mapped_val%10;
			tft_put_mega_ass_num(20, 1, display_val[2], BLACK);
		}
		
		tft_mega_update();
	}
}