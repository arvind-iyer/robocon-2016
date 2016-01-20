#include "main.h"

//uint16_t servo_val = SERVO_MIN;

int main(void) 
{
	led_init();
	ticks_init();
	can_init();
	can_rx_init();
	button_init();
	
	u8 state=0;
	while(1)
	{
		if(get_ticks()%200 == 0)
		{
			if(state==0)
			{
				led_control(LED_RED, 1);
				led_control(LED_BLUE, 0);
				led_control(LED_GREEN, 0);
				state = 1;
				//adc_value();
			}
			else if(state==1)
			{
				led_control(LED_RED, 0);
				led_control(LED_BLUE, 1);
				led_control(LED_GREEN, 0);
				state = 2;
			}
			else if(state==2)
			{
				led_control(LED_RED, 0);
				led_control(LED_BLUE, 0);
				led_control(LED_GREEN, 1);
				state = 0;
			}
		}
		
//		if(button_1()==0)
//		{
//			led_control(LED_RED,1);
//		}
//		else if(button_1()==1)
//		{
//			led_control(LED_RED,0);
//		}
	}
}