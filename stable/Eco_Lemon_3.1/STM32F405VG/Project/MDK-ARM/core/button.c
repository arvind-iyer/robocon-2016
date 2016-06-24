#include "button.h"

void button_init(){
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	
	GPIO_InitStruct.GPIO_Pin = BUTTON_4_PIN;
	GPIO_Init(BUTTON_4_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = BUTTON_1_PIN | BUTTON_3_PIN;
	GPIO_Init(BUTTON_1_3_GPIO, &GPIO_InitStruct);
	
	//Pull down for external buttons
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	GPIO_InitStruct.GPIO_Pin = BUTTON_5_PIN | BUTTON_6_PIN | BUTTON_7_PIN  | BUTTON_8_PIN;
	GPIO_Init(BUTTON_5_8_GPIO, &GPIO_InitStruct);
}

bool button_pressed(BUTTON button){
	if (button == BUT_4){
		return !GPIO_ReadInputDataBit(BUTTON_4_GPIO, button);
	}else if(button == BUT_1 || button == BUT_3){
		return !GPIO_ReadInputDataBit(BUTTON_1_3_GPIO, button);
	}else{
		return GPIO_ReadInputDataBit(BUTTON_5_8_GPIO, button);
	}
}
