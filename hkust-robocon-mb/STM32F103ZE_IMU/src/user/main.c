#include "main.h"

int main(void) 
{
	ticks_init();
	tft_easy_init();
	
	tft_put_logo(110, 90);
	
	s16 IMU_Buffer[6]={0x00}; 
	MPU6050_Initialize();
	
	while(1){
		MPU6050_GetRawAccelGyro(IMU_Buffer);   
		tft_clear();
		tft_append_line("%d", MPU6050_TestConnection());
		tft_append_line("Acceleration:");
		tft_append_line("X: %d", IMU_Buffer[0]);
		tft_append_line("Y: %d", IMU_Buffer[1]);
		tft_append_line("Z: %d", IMU_Buffer[2]);
		tft_append_line("RX: %d", IMU_Buffer[3]);
		tft_append_line("RY: %d", IMU_Buffer[4]);
		tft_append_line("RZ: %d", IMU_Buffer[5]);
		tft_update();
	}
}