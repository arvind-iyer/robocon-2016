#include "main.h"

//uint16_t servo_val = SERVO_MIN;

int main(void) 
{
	tft_init(0,BLACK,WHITE,WHITE);
	linear_ccd_init();
	while(1)
	{
		linear_ccd_read();
		tft_clear();

		for(u8 i=0;i<128;i++)
		{
			tft_put_pixel(i,linear_ccd_buffer[i],WHITE);
		}
		
		for(u8 j=0;j<128;j++)
		{
			tft_put_pixel(j,linear_ccd_buffer[j],BLACK);
		}
		
		tft_update();
	}
}