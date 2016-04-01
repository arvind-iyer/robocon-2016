#include "button.h"

void button_init(){
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_JS_UP | BUTTON_JS_DOWN | BUTTON_JS_LEFT | BUTTON_JS_RIGHT | BUTTON_JS_CENTRE;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INFRARED_SENSOR_GPIO, &GPIO_InitStructure);
}

u8 button_pressed(BUTTON button){
	return GPIO_ReadInputDataBit(BUTTON_JS_GPIO, button);
}