#include "linear_ccd.h"
u32 linear_ccd_buffer1[128];
u32 linear_ccd_buffer2[128];
u8 flag = 0;
u32 average = 0;
u8 l_edge = 0;
u8 r_edge = 0;
u8 mid = 0; 

void delay_57(void){
	for(int i = 0;i <4;++i){
		__asm("nop");
	}
}

void moving_adverage(u32 data[]){
	u32 a,b,c =0;
	for(int i =0;i<128;i++){
	if(i==0 || i == 127){
	}
		else{data[i] = (u32)((data[i-1] + data[i] + data[i+1])/3.0f);
		}
	}
}
	

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

	else{
		GPIO_ResetBits(SI1_PORT,SI1_PIN);
		GPIO_ResetBits(SI2_PORT,SI2_PIN);
	}
}

	


u32 AO1(){  // getting data from ccd1 ao1
	u32 temp = 0;
	int get_times = 250;
for(u8 i=0;i<get_times;i++){
temp += get_adc(1);
}
temp=(u32)(temp/get_times);
return temp;
}

u32 AO2(){  // getting data from ccd1 ao1
	u32 temp;
	int get_times = 250;
for(u8 i=0;i<get_times;i++){
temp += get_adc(2);
}
temp=(u32)(temp/get_times);
return temp;
}


void linear_ccd_read(){
	SI(1);
	delay_57();
	CLK(1);
	SI(0);
	linear_ccd_buffer1[0]=AO1();
    linear_ccd_buffer2[0]=AO2();
	delay_57();
	CLK(0);
	for(u8 n=1;n<128;n++)
	{
		CLK(1);		
		delay_57();
		linear_ccd_buffer1[n]=AO1();
		CLK(0);
		delay_57();
		linear_ccd_buffer2[n]=AO2();
		
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
    		for(u8 y=0;y<128;y++)
	{
	linear_ccd_buffer2[y] = (linear_ccd_buffer2[y])*160*2 / 4095;
	 if(linear_ccd_buffer2[y] >= 160){
		linear_ccd_buffer2[y] = 159;
	}
}

}






void linear_ccd_init()
{//initialization of clk 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(CLK_PORT, &GPIO_InitStructure);
	
//initialization of si1
	GPIO_InitStructure.GPIO_Pin = SI1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SI1_PORT, &GPIO_InitStructure);

//initialization of si2
	GPIO_InitStructure.GPIO_Pin = SI2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SI2_PORT, &GPIO_InitStructure);
}