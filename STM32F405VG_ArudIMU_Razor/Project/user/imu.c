#include "imu.h"

//The IMU will send back #SYNCH01\r\n
static u8 sync_progress = '#';
bool imu_synced = false;
bool imu_staged = false;
bool imu_pre_staged = false;
u8 imu_buffer[12] = {0};
s8 imu_buffer_pointer = 0;
float yaw_pitch_roll[3] = {0}; 
float start_ypr[3];

void IMU_receiver(u8 byte){
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
					//Finish syncing, init other parts
					sync_progress = '#';
					imu_synced = true;
					led_control(LED_D3, LED_ON);
					#ifdef IMU_USE_CONTINUOUS_MODE
						uart_tx(IMU_UART, "#o1#ob#oe0"); //Continuous binary output and request syncing
					#else
						uart_tx(IMU_UART, "#o0#ob#oe0#f"); //Discrete binary output upon sending #f, request syncing
					#endif 
					break;
			}
		}else{
			sync_progress = '#';
		}
	}else{
		//If sync is finished
		imu_buffer[imu_buffer_pointer] = byte;
		//Put received byte into a array, waiting to be resolved
		if (++imu_buffer_pointer == 12){
			imu_buffer_pointer = 0;
			#ifndef IMU_USE_CONTINUOUS_MODE
				//Request an additional frame when discrete mode is used
				uart_tx_byte(IMU_UART, '#');
				uart_tx_byte(IMU_UART, 'f');
			#endif
			if (imu_synced && !imu_staged){
				//Notify the main loop that syncing is ready
				imu_pre_staged = true;
			}
		}
	}
}

void sync_with_imu(){
	uart_tx(IMU_UART, "#s01"); //Request syncing
}

void imu_init(){
	uart_init(IMU_UART, 115200);
	uart_interrupt_init(IMU_UART, IMU_receiver);
	imu_synced = false;
}

u8 sync_time_count = 0;
void imu_update(){
	if (!imu_synced){
		sync_time_count += any_loop_diff;
		if (sync_time_count > SYNC_TIMEOUT){
			//If not synced, request syncing
			sync_with_imu();
			sync_time_count = 0;
		}
	}else{
			for (u8 i=0; i<3; i++){
				byte2float dataset;
				for (u8 k = (i*4); k < (i*4 + 4); k++){
					//Make 4 bytes into one float
					dataset.chars[(k%4)] = imu_buffer[k];
				}
				yaw_pitch_roll[i] = dataset.f;
				//Map the reading from -1~-179 and 0~179 to 0~360
				yaw_pitch_roll[i] = yaw_pitch_roll[i]<0.0f?yaw_pitch_roll[i]+360.0f:yaw_pitch_roll[i];
			}
			//For pitch and roll, map the values such that 1800 is the med pos
			yaw_pitch_roll[1] = yaw_pitch_roll[1] < 180.0f ? yaw_pitch_roll[1]+180.0f : (yaw_pitch_roll[1] > 180.0f ? yaw_pitch_roll[1] - 180.0f : 0);
			yaw_pitch_roll[2] = yaw_pitch_roll[2] < 180.0f ? yaw_pitch_roll[2]+180.0f : (yaw_pitch_roll[2] > 180.0f ? yaw_pitch_roll[2] - 180.0f : 0);
			
			if (imu_pre_staged && !imu_staged){
				//This part of code should only run once asap after pre staged(set after syncing)
				set_target(yaw_pitch_roll[0]);
				//Copy the starting yaw pitch roll to a private array
				memcpy(start_ypr, yaw_pitch_roll, 3 * sizeof(float));
				path_up_init();
				imu_staged = true;
			}
	}
}
