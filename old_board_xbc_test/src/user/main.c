#include "main.h"

int main(void)
{
		tft_init(1, RED, BLUE, WHITE);
	led_init();
				tft_clear();
			tft_prints(0, 0, "hi");
			tft_update();
		ticks_init();
		xbc_init(0);
		xbc_test_program();
		while(1){
			tft_clear();
			tft_prints(0, 0, "%d", get_ticks());
			tft_update();
		}
}