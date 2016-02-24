#include "imu.h"

//The IMU will send back #SYNCH01\r\n
u8 sync_progress = '#';
bool imu_synced = false;
u8 imu_buffer[12] = {0};
s8 imu_buffer_pointer = 0;
float yaw_pitch_roll[3] = {0}; 

void IMU_receiver(u8 byte){
	led_blink(LED_D2);
	//Sync with the IMU
	if (imu_synced == false){
		//Match the sync sequence
		if (byte == sync_progress){
			switch(byte){
				case '#':
					sync_progress = 'S';
					break;
				case 'S':
					sync_progress = 'Y';
					break;
				case 'Y':
					sync_progress = 'N';
					break;
				case 'N':
					sync_progress = 'C';
					break;
				case 'C':
					sync_progress = 'H';
					break;
				case 'H':
					sync_progress = '0';
					break;
				case '0':
					sync_progress = '1';
					break;
				case '1':
					sync_progress = '\r';
					break;
				case '\r':
					sync_progress = '\n';
					break;
				case '\n':
					sync_progress = '#';
					imu_synced = true;
					#ifdef IMU_USE_CONTINUOUS_MODE
						uart_tx(IMU_UART, "#o1#ob#oe0"); //Continuous binary output and request syncing
						tft_println("Ask: #o1#ob#oe0");
					#else
						uart_tx(IMU_UART, "#o0#ob#oe0"); //Discrete binary output upon sending #f, request syncing
						tft_println("Ask: #o0#ob#oe0");
					#endif 
					break;
			}
		}else{
			sync_progress = '#';
		}
	}else{
		//If sync is finished
		imu_buffer[imu_buffer_pointer] = byte;
		imu_buffer_pointer = (imu_buffer_pointer+1)%12;
		
	}
}

void sync_with_imu(){
	uart_tx(IMU_UART, "#s01"); //Discrete binary output upon sending #f, request syncing
}

void imu_init(){
	uart_init(IMU_UART, 115200);
	uart_interrupt_init(IMU_UART, IMU_receiver);
	imu_synced = false;
}

u8 sync_time_count = 0;
void imu_update(){
	if (!imu_synced){
		sync_time_count++;
		if (sync_time_count>10){
			sync_with_imu();
			sync_time_count = 0;
		}
	}else{
		uart_tx_byte(IMU_UART, '#');
		uart_tx_byte(IMU_UART, 'f');
	}
	
	typedef union {
		u8 chars[4];
		float f;
	} byte2float;
		
	for (u8 i=0; i<3; i++){
		byte2float dataset;

//		for (u8 k = (i*4); k < (i*4) + 4; k++){
//			dataset.chars[3-(k%4)] = imu_buffer[k];
//		}
		for (u8 k = (i*4); k < (i*4) + 4; k++){
			dataset.chars[(k%4)] = imu_buffer[k];
		}
		
		yaw_pitch_roll[i] = dataset.f;
	}
	
}
