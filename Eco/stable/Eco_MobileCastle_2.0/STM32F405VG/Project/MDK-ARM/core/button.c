#include "button.h"

void button_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	
	GPIO_InitStruct.GPIO_Pin = BUTTON_4_PIN;
	GPIO_Init(BUTTON_4_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN;
	GPIO_Init(BUTTON_1_3_GPIO, &GPIO_InitStruct);
}

bool button_pressed(BUTTON button){
	if (button == BUT_4){
		//return !GPIO_ReadInputDataBit(BUTTON_4_GPIO, button);
		return 0;
	}else{
		return !GPIO_ReadInputDataBit(BUTTON_1_3_GPIO, button);
	}
}
