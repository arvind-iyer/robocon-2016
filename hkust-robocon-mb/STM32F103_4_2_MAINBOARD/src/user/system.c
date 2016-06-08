#include "system.h"

void system_init(void){
	ticks_init();
	tft_init(2, BLUE, YELLOW, RED);
	button_init();
	led_init();
	servo_init();
	adc_init();
	gyro_init();
	gyro_cal();
}

void ls_test(void){
	static u32 pmin = 0, pmax = 0, max = 0, min = 9999; 
	
	tft_prints(0,0, "R1:%d", get_ls_adc_reading(0));
	tft_prints(0,1, "C1:%d", get_ls_cal_reading(0));
	tft_prints(0,2, "R2:%d", get_ls_adc_reading(1));
	tft_prints(0,3, "C2:%d", get_ls_cal_reading(1));
	tft_prints(0,4, "R3:%d", get_ls_adc_reading(2));
	tft_prints(0,5, "C3:%d", get_ls_cal_reading(2));
	
	max = (get_ls_adc_reading(0) > max)?get_ls_adc_reading(0):max;
	min = (get_ls_adc_reading(0) < min)?get_ls_adc_reading(0):min;
	
	if(get_full_ticks() > 2000){
		
		pmin = min;
		pmax = max;
	}
	tft_prints(0,6, "min: %d", pmin);
	tft_prints(0,7, "max: %d", pmax);
}

void gyro_test(void){
	tft_prints(0,0, "X: %d", get_X());
	tft_prints(0,1, "Y: %d", get_Y());
	tft_prints(0,2, "Ang: %d", get_angle());
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0){
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0);
		gyro_cal();
	}
}