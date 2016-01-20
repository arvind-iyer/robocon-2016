/**
* LED lights control program for controlling STM32F103VG Mainboard V4_2
**/

#include "led.h"

u8 led_state = 0;

u8 get_led_state(){
	return led_state;
}

/**
* @brief:	Initialize the gpio ports
**/
void led_init(void){
	gpio_init(LED_RED_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
	gpio_init(LED_BLUE_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
	gpio_init(LED_GREEN_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
}


/**
* @brief:		Control indivuial led lights
* @param led:	LED_RED/LED_BLUE/LED_GREEN binary representation, use | to operate multiple
* @param state:	LED_ON / LED_OFF
**/
void led_control(LED led, LED_STATE state){
	led_state = state ? led_state | led : ~(~led_state | led);

	if (led & LED_RED) {
		gpio_write(LED_RED_GPIO, (BitAction)state);
	}
	if (led & LED_BLUE) {
		gpio_write(LED_BLUE_GPIO, (BitAction)state);
	}
	if (led & LED_GREEN) {
		gpio_write(LED_GREEN_GPIO, (BitAction)state);
	}
}

/**
* @brief:		Reverse the state of the led
* @param led:	LED_RED/LED_BLUE/LED_GREEN binary representation, use | to operate multiple
**/
void led_blink(LED led){

	if (led & LED_RED) {
		led_control(LED_RED, !(led_state&LED_RED));
	}
	if (led & LED_BLUE) {
		led_control(LED_BLUE, !(led_state&LED_BLUE));
	}
	if (led & LED_GREEN) {
		led_control(LED_GREEN, !(led_state&LED_GREEN));
	}
}