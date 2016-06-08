#include "main.h"

int main(){
	system_init();
	while(1){
		tft_clear();
		gyro_test();
		tft_update();
	}
}	