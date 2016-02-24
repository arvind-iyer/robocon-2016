#include "led.h"

u8 led_state = 0;

u8 get_led_state(){
	return led_state;
}

void led_init(){
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Pin = LED_1 |LED_2|LED_3|LED_4|LED_5;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
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
			GPIO_WriteBit(LED_GPIO, LED_D3, (BitAction)state);
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
			led_control(LED_D1, (LED_STATE) !(led_state&LED_D1));
		}
	}
}
