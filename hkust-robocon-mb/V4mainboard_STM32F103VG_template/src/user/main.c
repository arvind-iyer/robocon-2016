#include "main.h"

int main(void) 
{
	ticks_init();
	tft_easy_init();
	encoder_init();

	tft_put_logo(110, 90);
	
	while(1){
		tft_clear();
		tft_append_line("%d", get_count(0));
		tft_append_line("%d", get_count(1));
		tft_update();
	}
}