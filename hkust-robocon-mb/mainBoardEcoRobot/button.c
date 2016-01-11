#include "button.h"
#include "buzzer_song.h"


void button_init(){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BUTTON1 | BUTTON2 | BUTTON3;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}
	
u8 read_button(u8 buttonId)
{
	if (buttonId <= 2)
		return	GPIO_ReadInputDataBit(BUTTON_GPIO, 1 << (13 + buttonId));
	else
		return 0;

}

void check_button(){

if(read_button(0)==0||read_button(1)==0||read_button(2)==0){
	FAIL_MUSIC;
}

}
