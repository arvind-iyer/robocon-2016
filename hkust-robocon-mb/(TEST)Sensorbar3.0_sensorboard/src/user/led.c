#include "led.h"
u32 (*led_on[16]) (GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void LED_init(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = FD10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA, FD10 );
	
	GPIO_InitStructure.GPIO_Pin = FD1 | FD2 | FD3 | FD4| FD5 | FD15 | LED_RED | LED_GREEN | LED_BLUE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_ResetBits(GPIOB, LED_BLUE | LED_RED | LED_GREEN);

	GPIO_InitStructure.GPIO_Pin = FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	
	GPIO_InitStructure.GPIO_Pin = FD6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOD, FD6);
}

/**
	* @breif Controls the tri-coloured LED
	* @param R: 1 ON, 0 OFF
	* @param G: 1 ON, 0 OFF
	* @param B: 1 ON, 0 OFF
**/
void LED_Control(u8 R, u8 G, u8 B)
{
	if(R)		GPIO_SetBits(GPIOB, LED_RED);
	else		GPIO_ResetBits(GPIOB, LED_RED);
	if(G)		GPIO_SetBits(GPIOB, LED_GREEN);
	else		GPIO_ResetBits(GPIOB, LED_GREEN);
	if(B)		GPIO_SetBits(GPIOB, LED_BLUE);
	else		GPIO_ResetBits(GPIOB, LED_BLUE);
}

/**
	* @brief Controls the Indicator LED
	* @param id: 0 - 30. odd number for turn on both LED, even number of turn on one LED 
	* @example id 0 for turning on FD0, id 1 for turning on both FD0 and FD1
**/
void IndicatorControl(u8 id){
	GPIO_SetBits(GPIOA, FD10);
	GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	GPIO_SetBits(GPIOD, FD6);
	
	if(id == 0)				{GPIO_ResetBits(GPIOC, FD0);}
	else if(id == 1)	{GPIO_ResetBits(GPIOC, FD0); GPIO_ResetBits(GPIOB, FD1);}
	
	else if(id == 2)	{GPIO_ResetBits(GPIOB, FD1);}
	else if(id == 3)	{GPIO_ResetBits(GPIOB, FD1); GPIO_ResetBits(GPIOB, FD2);}
	
	else if(id == 4)	{GPIO_ResetBits(GPIOB, FD2);}
	else if(id == 5)	{GPIO_ResetBits(GPIOB, FD2);GPIO_ResetBits(GPIOB, FD3);}
	
	else if(id == 6)	{GPIO_ResetBits(GPIOB, FD3);}
	else if(id == 7)	{GPIO_ResetBits(GPIOB, FD3);GPIO_ResetBits(GPIOB, FD4);}
	
	else if(id == 8)	{GPIO_ResetBits(GPIOB, FD4);}
	else if(id == 9)	{GPIO_ResetBits(GPIOB, FD4);GPIO_ResetBits(GPIOB, FD5);}
	
	else if(id == 10)	{GPIO_ResetBits(GPIOB, FD5);}
	else if(id == 11)	{GPIO_ResetBits(GPIOB, FD5);GPIO_ResetBits(GPIOD, FD6);}
	
	else if(id == 12)	{GPIO_ResetBits(GPIOD, FD6);}
	else if(id == 13)	{GPIO_ResetBits(GPIOD, FD6);GPIO_ResetBits(GPIOC, FD7);}
	
	else if(id == 14)	{GPIO_ResetBits(GPIOC, FD7);}
	else if(id == 15)	{GPIO_ResetBits(GPIOC, FD7);GPIO_ResetBits(GPIOC, FD8);}
	
	else if(id == 16)	{GPIO_ResetBits(GPIOC, FD8);}
	else if(id == 17)	{GPIO_ResetBits(GPIOC, FD8);GPIO_ResetBits(GPIOC, FD9);}
	
	else if(id == 18)	{GPIO_ResetBits(GPIOC, FD9);}
	else if(id == 19)	{GPIO_ResetBits(GPIOC, FD9);GPIO_ResetBits(GPIOA, FD10);}
	
	else if(id == 20)	{GPIO_ResetBits(GPIOA, FD10);}
	else if(id == 21)	{GPIO_ResetBits(GPIOA, FD10);GPIO_ResetBits(GPIOC, FD11);}
	
	else if(id == 22)	{GPIO_ResetBits(GPIOC, FD11);}
	else if(id == 23)	{GPIO_ResetBits(GPIOC, FD11);GPIO_ResetBits(GPIOC, FD12);}
	
	else if(id == 24)	{GPIO_ResetBits(GPIOC, FD12);}
	else if(id == 25)	{GPIO_ResetBits(GPIOC, FD12);GPIO_ResetBits(GPIOC, FD13);}
	
	else if(id == 26)	{GPIO_ResetBits(GPIOC, FD13);}
	else if(id == 27)	{GPIO_ResetBits(GPIOC, FD13);GPIO_ResetBits(GPIOC, FD14);}
	
	else if(id == 28)	{GPIO_ResetBits(GPIOC, FD14);}
	else if(id == 29)	{GPIO_ResetBits(GPIOC, FD14);GPIO_ResetBits(GPIOB, FD15);}
	
	else if(id == 30)	{GPIO_ResetBits(GPIOB, FD15);}
	
	
	else{
			GPIO_SetBits(GPIOA, FD10);
			GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
			GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
			GPIO_SetBits(GPIOD, FD6);
		}
}

void set_colour(u8 colour)
{
	switch(colour){
		case(RED):
			LED_Control(1,0,0);
			break;
		case(GREEN):
			LED_Control(0,1,0);
		break;
		case(BLUE):
			LED_Control(0,0,1);
			break;
		case(OFF):
			LED_Control(0,0,0);
		break;
	}
}