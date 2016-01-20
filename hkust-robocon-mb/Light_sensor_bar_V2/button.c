#include "button.h"

void button_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //button_2
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //button_1
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

u8 button_1()
{
	return(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15));
}

u8 button_2()
{
	return(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14));
}