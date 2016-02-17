#include "main.h"


u16 whiteline;
u16 ADC_val[4];
u8 sensor_pos=0;
u8 rx_flag = 120;

void receive(CanRxMsg msg)
{
	ADC_val[0] = msg.Data[0];
	ADC_val[0] = ADC_val[0] | (msg.Data[1] << 8);
	ADC_val[1] = msg.Data[2];
	ADC_val[1] = ADC_val[1] | (msg.Data[3] << 8);
	ADC_val[2] = msg.Data[4];
	ADC_val[2] = ADC_val[2] | (msg.Data[5] << 8);
	ADC_val[3] = msg.Data[6];
	ADC_val[3] = ADC_val[3] | (msg.Data[7] << 8);
}

void flag_test(CanRxMsg msg)
{
	ADC_val[0] = msg.Data[0];
	//ADC_val[1] = msg.Data[1];
	//ADC_val[2] = msg.Data[2];
	//ADC_val[3] = msg.Data[3];
}

void button_state()
{
	button_update();
	if(button_hold(BUTTON_JS_LEFT, 5,1))
		sensor_pos = 0;
	if(button_hold(BUTTON_JS_RIGHT, 5,1))
		sensor_pos = 1;
	if(button_hold(BUTTON_JS_UP, 5,1))
		sensor_pos = 2;
	if(button_hold(BUTTON_JS_DOWN, 5,1))
		sensor_pos = 3;
}

int main(void) 
{
	u8 display[16];
	
	tft_init(3,BLACK,WHITE,WHITE);
	can_init();
	can_rx_init();
	can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT , receive);
	//can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT , flag_test);
	ticks_init();
	button_init();
	while(1)
	{
		
			button_state();
			tft_clear();
			for(u8 i=0;i<4;i++)
				tft_prints(0,i,"%d", ADC_val[i]);
			//tft_prints(0,0, "%d", ADC_val[0]);
			tft_update();
		
		if(get_ticks()%200==0)
		{
			CAN_MESSAGE msg;
			msg.id = 0x032;
			msg.length = 1;
			msg.data[0] = 1;
			can_tx_enqueue(msg);
		}
	}
}