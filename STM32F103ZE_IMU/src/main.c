#include "main.h"

u16 ticks_img = 0;
u16 seconds_img = 0;

int main(void)
{
	ticks_init();
	tft_easy_init();
	tft_put_logo(110, 90);
	
	s16 IMU_Buffer[6]={0x00};
	MPU6050_I2C_Init();	
	MPU6050_Initialize();
	
	while(1){
		MPU6050_GetRawAccelGyro(IMU_Buffer);   
		tft_clear();
		tft_append_line("%d", get_ticks());
		tft_prints(0, 2, "AX: %d", IMU_Buffer[0]);
		tft_prints(0, 3, "AY: %d", IMU_Buffer[1]);
		tft_prints(0, 4, "AZ: %d", IMU_Buffer[2]);
		tft_prints(0, 5, "RX: %d", IMU_Buffer[3]);
		tft_prints(0, 6, "RY: %d", IMU_Buffer[4]);
		tft_prints(0, 7, "RZ: %d", IMU_Buffer[5]);
		tft_update();
	}
}