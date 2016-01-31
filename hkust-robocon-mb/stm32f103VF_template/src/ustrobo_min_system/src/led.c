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
	gpio_init(LED_D1_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
	gpio_init(LED_D2_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
	gpio_init(LED_D3_GPIO, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
}


/**
* @brief:		Control indivuial led lights
* @param led:	LED_D1/LED_D2/LED_D3 binary representation, use | to operate multiple
* @param state:	LED_ON / LED_OFF
**/
void led_control(LED led, LED_STATE state){
	led_state = state ? led_state | led : ~(~led_state | led);

	if (led & LED_D1) {
		gpio_write(LED_D1_GPIO, (BitAction)state);
	}
	if (led & LED_D2) {
		gpio_write(LED_D2_GPIO, (BitAction)state);
	}
	if (led & LED_D3) {
		gpio_write(LED_D3_GPIO, (BitAction)state);
	}
}

/**
* @brief:		Reverse the state of the led
* @param led:	LED_D1/LED_D2/LED_D3 binary representation, use | to operate multiple
**/
void led_blink(LED led){

	if (led & LED_D1) {
		led_control(LED_D1, !(led_state&LED_D1));
	}
	if (led & LED_D2) {
		led_control(LED_D2, !(led_state&LED_D2));
	}
	if (led & LED_D3) {
		led_control(LED_D3, !(led_state&LED_D3));
	}
}