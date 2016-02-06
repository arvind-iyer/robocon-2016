#include "helper.h"

/*
 * helper.c
 *
 * Author: Petel__
 * Copyright (c) 2014-2015 HKUST Robotics Team
 * 
 */

volatile uint16_t lastSystemPollingTime = 0;
volatile uint16_t lastSystemPollingCounter = 0;

void init(void)
{
	ticks_init();
	LED_INIT();
	GPIO_switch_init();
	buzzer_init();
	adc_init();
	linear_ccd_init();
	motor_init();
	button_init();
	//servo_init();
	
	buzzer_set_volume(BUZZER_volume);
	
	return ;
}

void systemPolling(void)
{
	if ((uint16_t)(get_real_ticks() - lastSystemPollingTime) >= 2000)
	{
		lastSystemPollingTime = get_ms_ticks();
		//buzzer_check();//must not romove£¬ otherwise no sound..haha
		//check_switch();
		//check_button();
		if (++lastSystemPollingCounter == 5){
			tft_update();
			lastSystemPollingCounter = 0;
		}
	}
}
