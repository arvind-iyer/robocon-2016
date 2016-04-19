#include "ultra_sonic.h"

void ultra_sonic_init()
{//initialization of TRIG 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);
	
//initialization of ECHO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
}

u32 call_ultra_sonic()
{
	u32 time = get_real_us();
	GPIOA->BSRR = TRIG_PIN;
	while(get_real_us()-time<=50);
	
	GPIOA->BRR = TRIG_PIN;
	GPIOA->BRR = ECHO_PIN; //set ECHO pin to 0
	
	time = get_real_us();
	while((GPIO_ReadInputDataBit(GPIOA, ECHO_PIN) != 1) && (get_real_us() - time <= 65535));
	
	time = get_real_us();
	while(GPIO_ReadInputDataBit(GPIOA, ECHO_PIN) == 1 && (get_real_us() - time <=65535));
	
	return get_real_us() - time;
}