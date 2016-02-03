#include "main.h"

int main(void) 
{
	ticks_init();
	tft_init(0, BLACK, WHITE, RED);
	encoder_init();

	tft_put_logo(110, 90);
	
	while(1){
		tft_clear();
		tft_append_line("%d", 1);
		tft_append_line("%d", 2);
		tft_append_line("%d", 3);
		tft_append_line("%d", 4);
		tft_append_line("%d", 5);
		tft_append_line("%d", 6);
		tft_append_line("%d", 7);
		tft_append_line("%d", 8);
		tft_append_line("%d", 9);
		tft_append_line("%d", 10);
		tft_append_line("%d", 11);
		tft_prints(0, 3,"%d", 6);
		tft_prints(0, 3,"%d", 7);
		tft_update();
	}
}