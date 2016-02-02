#include "main.h"


u16 whiteline;


void receive(CanRxMsg msg)
{
	CanRxMsg *pt = &msg;
	whiteline = Two_Bytes_into_u16(pt);
}

int main(void) 
{
	u8 display[16];
	
	tft_init(0,BLACK,WHITE,WHITE);
	can_init();
	can_rx_init();
	can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT , receive);
	ticks_init();
	while(1)
	{
		if(get_ticks()%200==0)
		{
			tft_clear();
			tft_prints(0,0,"%X", whiteline);
			tft_update();
		}
	}
}