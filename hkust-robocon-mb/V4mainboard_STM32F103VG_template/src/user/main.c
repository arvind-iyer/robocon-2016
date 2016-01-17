#include "main.h"

//uint16_t servo_val = SERVO_MIN;
u16 time=0;
u16 base_val = 1475;
int main(void) 
{
	tft_init(0,BLACK,WHITE,WHITE);
	linear_ccd_init();
	button_init();
	servo_init();
	ticks_init();
	
	u32 result1 = Sqrt(4);
	u32 result2 = Sqrt(69);
	u32 result3 = Sqrt(400);
	u32 result4 = Sqrt(13);
	u32 result5 = Sqrt(0);
	
	while(1)
	{
		tft_clear();
		tft_prints(0,0,"%d", result1);
		tft_prints(0,1,"%d", result2);
		tft_prints(0,2,"%d", result3);
		tft_prints(0,3,"%d", result4);
		tft_prints(0,4,"%d", result5);

		tft_update();
		
		
		
//		tft_clear();
//		if(get_ticks()%20==0)
//			button_update();
//		
//		if(button_hold(BUTTON_JS_LEFT, 0, 1))
//		{
//			base_val+=5;
//			tft_prints(0,0,"LEFT");
//		}
//		
//		if(button_hold(BUTTON_JS_RIGHT, 0, 1))
//		{
//			base_val-=5;
//			tft_prints(0,0,"RIGHT");
//		}
//		
//		if(base_val<500)
//			base_val=500;
//		
//		if(base_val>2450)
//			base_val=2450eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee;
//		
//		tft_prints(0,1,"%d", base_val);
//		tft_update();
//		servo_control(SERVO1,base_val);
	}
}