#include "main.h"

u16 data1[8];
u16 data2[8];

void receive(CanRxMsg msg)
{
	for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
}

void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}


int main(void) 
{
	u16 val = SERVO_MED;
	
	tft_init(0,BLACK,WHITE,WHITE);
	can_init();
	can_rx_init();
	can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
	while(1)
	{
		tft_clear();
        for(int i = 0; i < 8 ;i++){
            tft_prints(i,0,"%d",data1[i]);
        }
        for(int i = 0; i < 8; i++){
            tft_prints(i+8,0,"%d",data2[i]);
        }
        tft_prints(0,2,"First half: first array");
        tft_prints(0,3,"Second half: second array");
		tft_update();
        
	}
    return 0;
}

