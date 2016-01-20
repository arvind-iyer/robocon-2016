#include "main.h"

//uint16_t servo_val = SERVO_MIN;
u8 first;
u8 second;

void receive(CanRxMsg msg)
{
	first = msg.Data[0];
	second = msg.Data[1];
}

int main(void) 
{
	u16 val = SERVO_MED;
	
	tft_init(0,BLACK,WHITE,WHITE);
	can_init();
	can_rx_init();
	can_rx_add_filter(0x1B1,CAN_RX_MASK_EXACT , receive);
	while(1)
	{
		tft_clear();
		tft_prints(0,0,"first: %c", first);
		tft_prints(0,1,"second: %c", second);
		tft_update();
	}
}

