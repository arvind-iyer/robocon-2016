#include "linearccd.h"
u32 linear_ccd_buffer1[128];
u8 flag = 0;
u32 average = 0;
u8 l_edge = 0;
u8 r_edge = 0;
u8 mid = 0;

void delay_57(void) {
	for(int i = 0;i<4;++i) {
		__asm("nop");
	}
}

void CLK(u8 state){
	if(state == 1){
		GPIO_SetBits(clkPort, clkPin);
		state = 0;
	}
	if(state == 0){
		GPIO_ResetBits(clkPort, clkPin);
		state = 1;
	}
}
void SI(u8 bit){
	if(bit==1){
		GPIO_SetBits(siPort, siPin);
	}
	else{
		GPIO_ResetBits(siPort,siPin);
	}
}

u32 AO(){
	u32 temp = 0;
	int getTimes = 250;
	for(u8 i = 0; i<getTimes;i++){
		temp+= get_adc(1);
	}
	temp =(u32)(temp/getTimes);
	return temp;
}

void linearccdRead() {
	SI(0);
//	CLK(0);
//	_delay_us(1);
	SI(1);
	delay_57();
	CLK(1);
	SI(0);
	linear_ccd_buffer1[0]=AO();
	delay_57();
	CLK(0);
	for(u8 n=1;n<128;n++)
	{
		CLK(1);		
		delay_57();
		linear_ccd_buffer1[n]=AO();
		CLK(0);
		delay_57();
		
	}
	CLK(1);		
	delay_57();
	CLK(0);
	delay_57();	 
	
		for(u8 y=0;y<128;y++)
	{
	linear_ccd_buffer1[y] = (linear_ccd_buffer1[y])*160*2 / 4095;
	 if(linear_ccd_buffer1[y] >= 160){
		linear_ccd_buffer1[y] = 159;
	}
}


}

void linear_ccd_init()
{//initializatixon of clk 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = clkPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(clkPort, &GPIO_InitStructure);
	
//initialization of si
	GPIO_InitStructure.GPIO_Pin = siPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(siPort, &GPIO_InitStructure);
}
