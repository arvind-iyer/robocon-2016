#include "led.h"

static const GPIO* LED_GPIO_ARRAY[LED_COUNT] = {&LED_1_GPIO};

u8 led_state = 0;

u8 get_led_state(){
	return led_state;
}

void led_init(){
	gpio_init(&LED_1_GPIO, GPIO_Mode_OUT, GPIO_Fast_Speed, GPIO_OType_PP, GPIO_PuPd_NOPULL, false);
  gpio_write(&LED_1_GPIO, Bit_RESET);
}


 
/**
* @brief:		Control indivuial led lights
* @param led:	LED_D1-5 binary representation, use | to operate multiple
* @param state:	LED_ON / LED_OFF
**/
void led_control(LED led, LED_STATE state){
	led_state = state ? led_state | led : ~(~led_state | led);

	for (u8 i=0;i<LED_COUNT;i++){
		if (led & (1 << i)) {
			gpio_write(LED_GPIO_ARRAY[i], (BitAction)state);
		}
	}
}

/**
* @brief:		Reverse the state of the led
* @param led:	LED_D1-5 binary representation, use | to operate multiple
**/
void led_blink(LED led){
	for (u8 i=0;i<LED_COUNT;i++){
		if (led & (1 << i)) {
			led_control((LED)(1 << i), (LED_STATE) !(led_state&LED_D1));
		}
	}
}
