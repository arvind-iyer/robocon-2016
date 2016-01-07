#include "main.h"



u16 ticks_img 	= (u16)-1;
u16 seconds_img = (u16)-1;


int main(void)
{
	ticks_init();
	buzzer_init();
	tft_init(2, BLACK, WHITE, RED);
	gyro_init();
	bluetooth_init();
	bluetooth_rx_add_filter(0x00, 0xFF, hello);
	
	bluetooth_tx("Hello world\r\n");
	
	while (1) {
		if (ticks_img != get_ticks()) {
			ticks_img = get_ticks();
			if (ticks_img % 20 == 0) {
				bluetooth_update();
				tft_clear();
				tft_prints(0,0,"time: %d", get_seconds());
				tft_prints(0,1,"Bluetooth: %d", bluetooth_rx_state());
				tft_prints(0,2,"Position: %d", get_angle());
				tft_update();
			}
		}
	}
}


