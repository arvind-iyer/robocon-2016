#include "main.h"



u16 ticks_img 	= (u16)-1;
u16 seconds_img = (u16)-1;

int main(void)
{
	u8 x = 0;
	u8 y = 0;
	bluetooth_init();
	ticks_init();
	tft_init(1, BLACK, RED, BLUE);
	while(1){
		bluetooth_update();
		if (bluetooth_rx_state() != 0){
			if (bluetooth_rx_state() != 8){
				tft_prints(x % 4, y, "%c", bluetooth_rx_state());
				x++;
				if (x>4){
					x = 0;
					y++;
				}
				tft_update();
			}else{
				x--;
				if (x<0){
					x = 4;
					y--;
				}
				tft_prints(x % 4, y, "%c", ' ');
			}
		}
	}
}