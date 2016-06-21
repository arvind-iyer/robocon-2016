#include "led.h"


void led_init(){
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //enable the clock
	
	/* Fill in the init Structure */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LED_1 |LED_2|LED_3|LED_4|LED_5;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	
	/* Turn off all the LED */
	LED_OFF_ALL();
}


 
void LED_OFF(uint16_t gpio_pin){
	GPIO_ResetBits(LED_GPIO,gpio_pin);
}

void LED_ON(uint16_t gpio_pin){
	GPIO_SetBits(LED_GPIO,gpio_pin);
}

void LED_ON_ALL(){
    GPIO_SetBits(LED_GPIO,LED_1|LED_2|LED_3|LED_4|LED_5);
}

void LED_OFF_ALL(){
    GPIO_ResetBits(LED_GPIO,LED_1|LED_2|LED_3|LED_4|LED_5);
}
