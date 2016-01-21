#include "button.h"

void button_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15; //button_2
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

u8 button_1()
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
}

u8 button_2()
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
}