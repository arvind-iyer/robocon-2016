#include "pneumatic_control.h"

void pneumatic_init()
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_5|GPIO_Pin_6;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

void pneumatic_control(GPIO_TypeDef* PORT,u16 gpio_pin,u8 state)
{
	
if (state==1){GPIO_SetBits(PORT,gpio_pin);}//ON
else {GPIO_ResetBits(PORT,gpio_pin);}      //OFF

}
