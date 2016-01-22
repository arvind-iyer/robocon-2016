#include "main.h"

//uint16_t servo_val = SERVO_MIN;
u8 whiteline_pos[16];
u8 ok=0;
u8 flag11 = 99;

void receive(CanRxMsg msg)
{
	u8 tx_flag = msg.Data[0];
	flag11 = tx_flag;
	if(tx_flag==0)
	{
		for(u8 i=0;i<7;i++)
			whiteline_pos[i] = msg.Data[i+1];
		
		ok = 0;
	}
	if(tx_flag==1)
	{
		for(u8 i=0;i<7;i++)
			whiteline_pos[i+7] = msg.Data[i+1];
		
		ok = 0;
	}
	if(tx_flag==2)
	{
		for(u8 i=0;i<4;i++)
			whiteline_pos[i+13] = msg.Data[i+1];
		
		ok = 1;
	}
}

int main(void) 
{
	u16 val = SERVO_MED;
	
	tft_init(0,BLACK,WHITE,WHITE);
	can_init();
	can_rx_init();
	can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT , receive);
	while(1)
	{
		tft_clear();
		
		tft_prints(1,1,"flag %d", flag11);
		if(ok==1)
		{
			for(u8 i=0;i<16;i++)
			tft_prints(i,0,"%d", whiteline_pos[i]);
			
		}
		
		tft_update();
	}
}

