#include "button.h"

void button_init(){
    TM_GPIO_Init(BUTTON_JS_GPIO, BUTTON_PIN, 
			TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Fast);
}

bool button_pressed(BUTTON button){
	return !TM_GPIO_GetInputPinValue(BUTTON_JS_GPIO, button);
}
