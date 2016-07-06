#include "button.h"

static const GPIO* BUTTONS[BUTTONS_COUNT] = {&PC10, &PC11, &PC12, &PD2, &PA4, &PA5, &PA6, &PA7};

void button_init(){
	
	//Pull up for on-board buttons
	for (u8 i=0;i<4;i++){
		gpio_init(BUTTONS[i], GPIO_Mode_IN, GPIO_Fast_Speed, GPIO_OType_PP, GPIO_PuPd_UP, true);
	}
	
	//Pull down for external buttons
	for (u8 i=4;i<BUTTONS_COUNT;i++){
		gpio_init(BUTTONS[i], GPIO_Mode_IN, GPIO_Fast_Speed, GPIO_OType_PP, GPIO_PuPd_DOWN, true);
	}
}

bool button_pressed(BUTTON_ID button_id){
	return gpio_read_input(BUTTONS[button_id]);
}
