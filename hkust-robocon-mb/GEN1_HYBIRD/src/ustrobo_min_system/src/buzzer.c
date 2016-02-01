#include "buzzer.h"

s16 beep_duration = 0;
u32 last_ticks = 0;

void buzzer_init(void)
{		   	
	GPIO_InitTypeDef BUZZER_InitStructure;
	RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	BUZZER_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  BUZZER_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	BUZZER_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
	GPIO_Init(BUZZER_GPIO_PORT, &BUZZER_InitStructure);
	
	GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

void buzzer_on(){
	GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

void buzzer_beep(u16 duration){
	GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
	beep_duration = duration;
}

void buzzer_off(){
	GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

//To be called at every ticks
void buzzer_update(){
	if (beep_duration>0){
		if (--beep_duration<=0){
			GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
		}
	}
}