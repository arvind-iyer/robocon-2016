#include "linear_ccd.h"

void CLK(u8 state){//self make clock to control two ccd

		if (state == 1){
	GPIO_SetBits(CLK_PORT, CLK_PIN);
		state=0;
	}
	else if (state == 0){
	GPIO_ResetBits(CLK_PORT,CLK_PIN);
		state=1;
	}
}


void SI(u8 bit){ //controlling Linear_ccd 1  and 2 si1 si2

	if(bit==1){
	GPIO_SetBits(SI1_PORT,SI1_PIN);
	GPIO_SetBits(SI2_PORT,SI2_PIN);
	}

	else
	{
		GPIO_ResetBits(SI1_PORT,SI1_PIN);
		GPIO_ResetBits(SI2_PORT,SI2_PIN);
	}
}

	


u32 AO1()	// getting data from ccd1 ao1
{  
	u32 temp;
	int get_times = 200;
	for(u8 i=0;i<get_times;i++)
		temp += get_adc(1);
	temp=(u32)(temp/get_times);
	return temp;
}

u32 AO2()	// getting data from ccd1 ao2
{  
	u32 temp;
	int get_times = 10;
	for(u8 i=0;i<get_times;i++)
		temp += get_adc(2);
	temp=(u32)(temp/get_times);
	return temp;
}

u16 linear_ccd_buffer1[128];
u16 linear_ccd_buffer2[128];
u8 flag = 0;

void linear_ccd_read()
{
	u16 temp1[128],temp2[128];
	SI(0);
	CLK(0);
	_delay_us(1);
	SI(1);
	for(u8 n=0;n<128;n++)
	{
		_delay_us(1);
		CLK(1);		
		SI(0);
		_delay_us(1);
		temp1[n]=AO1();
		temp2[n]=AO2();
		CLK(0);
	}
	
	for (u8 i=1;i<127;i++){
		linear_ccd_buffer1[i] = MAX(MIN(temp1[i-1],temp1[i]),MIN(MAX(temp1[i-1],temp1[i]),temp1[i+1])); //Locate the mid point
		linear_ccd_buffer2[i] = MAX(MIN(temp2[i-1],temp2[i]),MIN(MAX(temp2[i-1],temp2[i]),temp2[i+1])); //Locate the mid point
	}
	
}


void linear_ccd_init()
{//initialization of clk 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CLK_PORT, &GPIO_InitStructure);
	
//initialization of si1
	GPIO_InitStructure.GPIO_Pin = SI1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI1_PORT, &GPIO_InitStructure);

//initialization of si2
	GPIO_InitStructure.GPIO_Pin = SI2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI2_PORT, &GPIO_InitStructure);
}
