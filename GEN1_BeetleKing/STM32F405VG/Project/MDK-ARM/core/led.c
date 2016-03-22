#include "led.h"

static u16 LED_GPIO_ARRAY[LED_COUNT] = {LED_1, LED_2, LED_3, LED_4, LED_5};

u8 led_state = 0;

u8 get_led_state(){
	return led_state;
}

void led_init(){
    TM_GPIO_Init(LED_GPIO,LED_1|LED_2|LED_3|LED_4|LED_5,TM_GPIO_Mode_OUT,TM_GPIO_OType_PP,TM_GPIO_PuPd_NOPULL,TM_GPIO_Speed_Fast);
    GPIO_ResetBits(LED_GPIO, LED_1);
    GPIO_ResetBits(LED_GPIO, LED_2);
    GPIO_ResetBits(LED_GPIO, LED_3);
    GPIO_ResetBits(LED_GPIO, LED_4);
    GPIO_ResetBits(LED_GPIO, LED_5);
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
			GPIO_WriteBit(LED_GPIO, LED_GPIO_ARRAY[i], (BitAction)state);
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
			led_control(1 << i, (LED_STATE) !(led_state&LED_D1));
		}
	}
}
