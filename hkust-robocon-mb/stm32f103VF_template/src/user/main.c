#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

int main(void)
{
	tft_init(2, BLUE, WHITE, BLACK);
	encoder_init();
	gyro_init();
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	_delay_ms(4000);
	
	auto_init();
	//auto_tar_enqueue(0, 1500, 0, 0.0, true);
	
	/*
	//8-figure
	auto_tar_enqueue(500, 0, 0, 0.0, false);
	auto_tar_enqueue(1000, 500, 0, -2.0, false);
	auto_tar_enqueue(500, 1000, 0, -2.0, false);
	auto_tar_enqueue(0, 500, 0, -2.0, false);
	auto_tar_enqueue(0, -500, 0, 0.0, false);
	auto_tar_enqueue(-500, -1000, 0, 2.0, false);
	auto_tar_enqueue(-1000, -500, 0, 2.0, false);
	auto_tar_enqueue(-500, 0, 0, 2.0, false); 
	auto_tar_enqueue(0, 0, 0, 0.0, true);
	*/
	
	//circle
	auto_tar_enqueue(1000, 1000, 0, 1.0, true);
	auto_tar_enqueue(2000, 0, 0, 1.0, true);
	auto_tar_enqueue(1000, -1000, 0, 1.0, true);
	auto_tar_enqueue(0, 0, 0, 1.0, true);
		
	while (1) {
		if (get_ticks() % 50 == 0) {
			auto_var_update();			
			auto_motor_update();
			//auto_calibrate();
		}
	}
}
