#include "LEDs.h"
#define LED_L  GPIO_Pin_4
#define LED_M  GPIO_Pin_3
#define LED_R  GPIO_Pin_15

void LED_INIT(){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_M | LED_L;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED_R;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
        GPIO_ResetBits(GPIOA, LED_R);
	GPIO_ResetBits(GPIOB, LED_M);
	GPIO_ResetBits(GPIOB, LED_L);
}
	

u8 state=0;
 void LED_blink(GPIO_TypeDef* PORT,u16 gpio_pin){

	if(state==0){
	GPIO_ResetBits(PORT,gpio_pin);
	state=1;
	}
	else if(state==1)
	{
		GPIO_SetBits(PORT,gpio_pin);
	state=0;
	}

}
 
void LED_OFF(GPIO_TypeDef* PORT,u16 gpio_pin){
	GPIO_ResetBits(PORT,gpio_pin);
}

void LED_ON(GPIO_TypeDef* PORT,u16 gpio_pin){
	GPIO_SetBits(PORT,gpio_pin);
}