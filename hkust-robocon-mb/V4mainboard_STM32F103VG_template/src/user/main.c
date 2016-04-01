#include "main.h"

int main(void) 
{
	tft_init(1,BLACK,WHITE,WHITE);
	ticks_init();
	uart_init(COM3,19200);
	switch (LRF_init()){
		case ECHO_NOT_MATCH:
			tft_prints(0,1,"ECHO NOT MATCH");
			break;
		case SCIP2_NOT_SUCESS:
			tft_prints(0,1,"SCIP2 NOT SUCESS");
			break;
		case SUM_NOT_EQUAL:
			tft_prints(0,1,"SUM NOT EQUAL");
			break;
		case LASER_MALFUNCTION:
			tft_prints(0,1,"LASER MALFUNCTION");
			break;
		default:
			tft_prints(0,1,"FUCK YOU!!");
			break;
	}
	tft_update();
	while(1);
}

