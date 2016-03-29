#include "imu.h"

//The IMU will send back #SYNCH01\r\n
static u8 sync_progress = '#';
bool imu_synced = false;
bool imu_staged = false;
bool imu_pre_staged = false;
u8 imu_buffer[12] = {0};
s8 imu_buffer_pointer = 0;
float out_ypr[3] = {0}; 
float cal_ypr[3] = {0}; 
float start_ypr[3];

static float yaw_samples[SAMPLE_SIZE] = {0};
static u8 sample_size = 0;
float yaw_bias = 0;

void IMU_receiver(u8 byte){

	//Sync with the IMU
	if (imu_synced == false){
		//Match the sync sequence
		if (byte == sync_progress){
			switch(byte){
				case '#':
					sync_progress = 'S';
                    tft_prints(0,8,"S");
                    tft_update();
					break;
				case 'S':
					sync_progress = 'Y';
                tft_prints(0,8,"Y");
                tft_update();
					break;
				case 'Y':
					sync_progress = 'N';
                tft_prints(0,8,"N");
                tft_update();
					break;
				case 'N':
					sync_progress = 'C';
                tft_prints(0,8,"C");
                tft_update();
					break;
				case 'C':
					sync_progress = 'H';
                tft_prints(0,8,"H");
                tft_update();
					break;
				case 'H':
					sync_progress = '0';
                tft_prints(0,8,"0");
                tft_update();
					break;
				case '0':
					sync_progress = '1';
                tft_prints(0,8,"1");
                tft_update();
					break;
				case '1':
					sync_progress = '\r';
                tft_prints(0,8,"\r");
                tft_update();
					break;
				case '\r':
					sync_progress = '\n';
                tft_prints(0,8,"\n");
                tft_update();
					break;
				case '\n':
					//Finish syncing, init other parts
					sync_progress = '#';
					imu_synced = true;
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
    for (u8 i=0; i<3; i++){
        byte2float dataset;
        for (u8 k = (i*4); k < (i*4 + 4); k++){
            //Make 4 bytes into one float
            dataset.chars[(k%4)] = imu_buffer[k];
        }
        cal_ypr[i] += abs_diff(dataset.f, out_ypr[i]);
        out_ypr[i] = dataset.f;
    }
}
