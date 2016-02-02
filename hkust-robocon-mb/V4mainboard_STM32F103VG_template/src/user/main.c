#include "main.h"

//uint16_t servo_val = SERVO_MIN;
//u8 whiteline_pos[16];
//u8 ok=0;
//u8 flag11 = 99;
//extern s16 *Accelpt;
//extern u8 *I2C_pt;
//void receive(CanRxMsg msg)
//{
//	u8 tx_flag = msg.Data[0];
//	flag11 = tx_flag;
//	if(tx_flag==0)
//	{
//		for(u8 i=0;i<7;i++)
//			whiteline_pos[i] = msg.Data[i+1];
//		
//		ok = 0;
//	}
//	if(tx_flag==1)
//	{
//		for(u8 i=0;i<7;i++)
//			whiteline_pos[i+7] = msg.Data[i+1];
//		
//		ok = 0;
//	}
//	if(tx_flag==2)
//	{
//		for(u8 i=0;i<3;i++)
//			whiteline_pos[i+13] = msg.Data[i+1];
//		
//		ok = 1;
//	}
//}

int main(void) 
{
	s16 val[6];
	
	tft_init(1,BLACK,WHITE,WHITE);
	ticks_init();
	//bluetooth_init();
	//gyro3d_init();
	//can_init();
	//can_rx_init();
	//can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT , receive);
	MPU6050_I2C_Init();
	MPU6050_Initialize();
	while(1)
	{
		if(get_ticks()%200==0)
		{
			MPU6050_GetRawAccelGyro(val);
			tft_clear();
			
			for(u8 i=0;i<6;i++)
				tft_prints(i,0,"%d", *(val+i));
			
			//tft_prints(3,3,"Diu");
		
			tft_update();
			
			//bluetooth_tx_byte(MPU6050_TestConnection());
			//bluetooth_tx_byte(' ');
		}
	}
}

