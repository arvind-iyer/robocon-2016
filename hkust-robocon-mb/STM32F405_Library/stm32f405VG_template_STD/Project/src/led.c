#include "led.h"


void led_init(){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LED_1 |LED_2|LED_3|LED_4|LED_5;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	
    GPIO_ResetBits(LED_GPIO, GPIO_Pin_2);
    GPIO_ResetBits(LED_GPIO, GPIO_Pin_3);
    GPIO_ResetBits(LED_GPIO, GPIO_Pin_4);
    GPIO_ResetBits(LED_GPIO, GPIO_Pin_5);
    GPIO_ResetBits(LED_GPIO, GPIO_Pin_6);
}


 
void LED_OFF(uint16_t gpio_pin){
	GPIO_ResetBits(LED_GPIO,gpio_pin);
}

void LED_ON(uint16_t gpio_pin){
	GPIO_SetBits(LED_GPIO,gpio_pin);
}