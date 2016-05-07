#include "pneumatic.h"

void pneumatic_init(){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void pneumatic_control(GPIO_TypeDef* PORT,u16 gpio_pin,u8 state){
	
	if (state==1){
		GPIO_SetBits(PORT,gpio_pin);
	}
	else {
		GPIO_ResetBits(PORT,gpio_pin);
	} 

}