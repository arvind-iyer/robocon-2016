#include "imu.h"

//The IMU will send back #SYNCH01\r\n
u8 sync_progress = '#';
bool synced = false;
u8 imu_buffer[12] = {0};
u8 imu_buffer_pointer = 0;

void IMU_receiver(u8 byte){
	led_blink(LED_D2);
	//Sync with the IMU
	if (synced == false){
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
					synced = true;
					break;
			}
		}else{
			sync_progress = '#';
		}
	}else{
		//If sync is finished
		imu_buffer[imu_buffer_pointer] = byte;
		imu_buffer_pointer = (imu_buffer_pointer + 1) % 12;
	}
}

void sync_with_imu(){
	#ifdef IMU_USE_CONTINUOUS_MODE
		uart_tx(COM3, "#o1#ob#s01"); //Continuous binary output and request syncing
		tft_println("Ask: #o1#ob#s01");
	#else
		uart_tx(COM3, "#o0#ob#s01"); //Discrete binary output upon sending #f, request syncing
		tft_println("Ask: #o0#ob#s01");
	#endif
}

void imu_init(){
	uart_init(COM3,115200);
	uart_interrupt(COM3);
	uart_interrupt_init(COM3, IMU_receiver);
	synced = false;
}

void imu_update(){
	tft_clear();
	tft_println(synced?"SYNCED":"NOT SYNCED");
	if (!synced){
		sync_with_imu();
	}
	tft_println("This Loop: %d", this_loop_ticks);
	tft_println("Interval: %d", this_loop_ticks - last_long_loop_ticks);
	tft_println("%d %d %d", imu_buffer[0], imu_buffer[1], imu_buffer[2], imu_buffer[3]);
	tft_println("%d %d %d", imu_buffer[4], imu_buffer[5], imu_buffer[6], imu_buffer[7]);
	tft_println("%d %d %d", imu_buffer[8], imu_buffer[9], imu_buffer[10], imu_buffer[11]);
	tft_update();
}
