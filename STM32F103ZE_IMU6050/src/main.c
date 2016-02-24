#include "main.h"
#include "stdbool.h"

u16 ticks_img = 0;
u16 seconds_img = 0;

int main(){
	ticks_init();
	tft_easy_init();
	tft_put_logo(90, 110);
	u8 state = 0;
	
	s16 IMU_Buffer[IMU_BUFFER_COUNT][6]={0x00};
	s16 IMU_Output[6] = {0x00};
	u8 IMU_Buffer_index = 0;
	s32 IMU_Ground[6]={0x00};
	u16 ground_buffer_count = 0;
	u16 servo_val = 1500;
	
	MPU6050_I2C_Init();	
	MPU6050_Initialize();
	
	while(1){
		MPU6050_GetRawAccelGyro(IMU_Buffer[IMU_Buffer_index]);   
		tft_clear();
		tft_append_line("%d", get_ticks());
		
		if(get_seconds()<3){
			tft_clear();
			if (state == 0){
				tft_append_line("Waiting.");
			}else if (state == 1){
				tft_append_line("Waiting..");
			}else{
				tft_append_line("Waiting...");
			}
			state = (state+1)%3;
			if(get_seconds()>1){
				for (u8 i=0;i<6;i++){
					IMU_Ground[i] += IMU_Buffer[0][i];
				}
				ground_buffer_count++;
			}
			tft_append_line("%d", ground_buffer_count);
		}else if(state<3){
			for (u8 i=0;i<6;i++){
				IMU_Ground[i] /= ground_buffer_count;
			}
			state = 3;
		}else {
			IMU_Buffer_index = (IMU_Buffer_index+1)%IMU_BUFFER_COUNT;
		}
		
		for (u8 i=0;i<6;i++){
			s32 Temp_Buffer = 0;
			for (u8 k=0;k<IMU_BUFFER_COUNT;k++){
				Temp_Buffer += IMU_Buffer[k][i];
			}
			IMU_Output[i] = Temp_Buffer/IMU_BUFFER_COUNT;
		}
		
		if (IMU_Output[3]>1500){
			servo_val = 1800;
		}else if(IMU_Output[3]<-1500){
			servo_val = 1200;
		}
		
		tft_append_line("Servo: %d", servo_val);
		tft_append_line("AX: %d", IMU_Output[0]);
		tft_append_line("AY: %d", IMU_Output[1]);
		tft_append_line("AZ: %d", IMU_Output[2]);
		tft_append_line("RX: %d", IMU_Output[3]);
		tft_append_line("RY: %d", IMU_Output[4]);
		tft_append_line("RZ: %d", IMU_Output[5]);
		tft_update();
	}
}