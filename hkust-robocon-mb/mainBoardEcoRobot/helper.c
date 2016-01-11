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
	button_init();
    tft_spi_init();
	pneumatic_init();
    servo_init();
	buzzer_set_volume(BUZZER_volume);
	
#if (defined(LCD_orientation_index) && defined(LCD_background_color) && defined(LCD_text_color) && defined(LCD_special_color))
	tft_init(LCD_orientation_index, LCD_background_color, LCD_text_color, LCD_special_color);
#else
	tft_prints(0, 0, "Not found!3");
	tft_update();
	buzzer_control_note(1, 10, NOTE_G, 6);
	failLoop(100, GPIOA, GPIO_Pin_15);
#endif
	
#if (defined(UART_port) && defined(UART_buadrate))
	uart_init(UART_port, UART_buadrate);
#else
	tft_prints(0, 0, "Not found!2");
	tft_update();
	buzzer_control_note(2, 10, NOTE_G, 6);
	failLoop(100, GPIOB, GPIO_Pin_3);
#endif

#if defined(UART_printf_port)
	//uart_printf_enable(UART_printf_port);
#else
	tft_prints(0, 0, "[Printf Port]");
	tft_update();
	buzzer_control_note(3, 10, NOTE_G, 6);
	failLoop(100, GPIOB, GPIO_Pin_4);
#endif
	
	MARIO_START;
	
	return ;
}

void systemPolling(void)
{
	if ((uint16_t)(get_ms_ticks() - lastSystemPollingTime) >= 10)
	{
		lastSystemPollingTime = get_ms_ticks();
		buzzer_check();//must not romove£¬ otherwise no sound..haha
		//check_switch();
		//check_button();
		if (++lastSystemPollingCounter == 5)
		{
			tft_update();
			lastSystemPollingCounter = 0;
		}
	}
}




void failLoop(uint16_t freq, GPIO_TypeDef* PORT,u16 gpio_pin)
{
	uint16_t lastTime = 0;
	while (true)
	{
		if (get_ms_ticks() - lastTime >= freq)
		{
			lastTime = get_ms_ticks();
			LED_blink(PORT, gpio_pin);	
		}
		_delay_ms(freq);
	}
}
