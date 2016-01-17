#include "button.h"

void button_init(){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BUTTON1 | BUTTON2;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
	
u8 read_button(u8 buttonId)
{
	if (buttonId <= 1)
		return	GPIO_ReadInputDataBit(BUTTON_GPIO, 1 << (6 + buttonId));
	else
		return 0;

}

void check_button(){

}
