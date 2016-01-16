#include "holder.h"
#include "ultra_sonic.h"
#include "motor.h"
#include "pneumatic_control.h"
#include "line_track.h"


void tubes_check_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = left_pin;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = right_pin;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
}

//void slow_line_track()
//{
//	u8 ccd1_mids[2]={0}, ccd2_mids[2]={0};	//mid-pt of white lines detected (at most 2 white lines so size=2)			
//	u8 i,index=0;
//	u16 time=get_real_ticks();
//	u8 SHIT_IS_REAL = 0;
//	
//	linear_ccd_read();
//		
//	for(i=LEFT;i<RIGHT;i++)
//	{
//		if(linear_ccd_buffer1[i]>white_threshold1)
//		{
//			ccd1_mids[index]=i;
//			for(;i<RIGHT&&linear_ccd_buffer1[i]>black_threshold1;i++);
//			if(ccd1_mids[index]<LEFT+WIDTH && i>RIGHT-WIDTH)
//				ccd1_mids[index]=ALL_WHITE;
//			else
//			{
//				ccd1_mids[index]=(ccd1_mids[index]+i)/2;
//				index++;
//			}
//			/*if(i-ccd1_mids[index]>=WIDTH)
//			{
//				if(i-ccd1_mids[index]>6*WIDTH)
//				{
//					if((i+ccd1_mids[index])/2 > 64)
//						track_state = R90;
//					else
//						track_state = L90;
//				}
//				else
//				{
//					ccd1_mids[index]=(ccd1_mids[index]+i)/2;
//					index++;
//				}
//			}
//			else
//				ccd1_mids[index]=i;*/
//		}
//	}
//	index=0;
//	for(i=LEFT;i<RIGHT;i++)
//	{
//		if(linear_ccd_buffer2[i]>white_threshold2)
//		{
//			ccd2_mids[index]=i;
//			for(;i<RIGHT&&linear_ccd_buffer2[i]>black_threshold2;i++);
//			if(ccd2_mids[index]<LEFT+WIDTH && i>RIGHT-WIDTH)
//				ccd2_mids[index]=ALL_WHITE;
//			else
//			{
//				ccd2_mids[index]=(ccd2_mids[index]+i)/2;
//				index++;
//			}
//		}
//	}
//	
//	
//	
//	u8 mid1 = MAX(MIN(mid[0][0],mid[0][1]),MIN(MAX(mid[0][0],mid[0][1]),ccd1_mids[0])); 
//	u8 mid2 = MAX(MIN(mid[1][0],mid[1][1]),MIN(MAX(mid[1][0],mid[1][1]),ccd2_mids[0]));
//	
//	u8 power;
//	u8 check;
//	u8 temp = past[0];
//	for(check=1;check<100;check++)
//		if(past[check]!=temp)
//			break;
//	if(check==100)
//		power=2;
//	else
//		power=1;
//	
//	if(ccd2_mids[0]==ALL_WHITE)
//	{
//		motor_control(1,1,1);
//		motor_control(1,0,1);
//	}
//	
//	if(ccd1_mids[0]==ALL_WHITE)
//	{
//		rotate++;
//		if(rotate == 1)
//		{
//			left_turn = 1;
//		}
//		else if(rotate == 2)
//		{
//			front = 0;
//		}

//		if(left_turn)
//		{
//			motor_control(1,0, 120);
//			motor_control(2,0, 120);
//			
//			if((ccd1_mids[0]==0)&&(ccd2_mids[0]==0))
//			{
//				motor_control(1,0,120);
//				motor_control(2,1,120);
//			}
//			
//			if(ccd1_mids[0] == 64)
//			{
//				left_turn = 0;
//				rotate = 2;
//				front = 0;
//			}
//		}
//	}
//	else if(ccd2_mids[0]==0)
//	{
//		motor_control(1,0,120);
//		motor_control(2,1,120);
//	}
//	else if(!rotate)
//	{
//		u8 left = (100+mid2-64);
//		u8 right = (100-mid2+64);
//		if(front==1)
//		{
//			motor_control(1,1,left<100?100:left>150?150:left*power);
//			motor_control(2,0,right<100?100:right>150?150:right*power);
//		}
//		else
//		{
//			motor_control(1,0,left<100?100:left>150?150:left*power);
//			motor_control(2,1,right<100?100:right>150?150:right*power);
//		}
//		mid[0][pos]=ccd1_mids[0];
//		mid[1][pos]=ccd2_mids[0];
//		pos=(pos+1)%2;
//	}
//	
//	//Display
//	tft_clear();
//	tft_prints(0,0,"TOP");
//	tft_prints(0,1,"Tracking...");
//	tft_prints(0,2,"MID1: %d %d", mid[0][0],mid[0][1]);
//	tft_prints(0,3,"MID2: %d %d", mid[1][0],mid[1][1]);
//	
//	for(u8 i=0;i<128;i++)
//	{
//		tft_put_pixel(i,159-white_threshold2*79/4095,YELLOW);
//		tft_put_pixel(i,159-black_threshold2*79/4095,BLUE);
//		tft_put_pixel(i,159-linear_ccd_buffer1[i]*79/4095,GREEN);
//		tft_put_pixel(i,159-linear_ccd_buffer2[i]*79/4095,WHITE);
//	}
//	for(u8 i=0;i<128;i++)
//	{
//		tft_put_pixel(i,159-linear_ccd_buffer1[i]*79/4095,BLACK);
//		tft_put_pixel(i,159-linear_ccd_buffer2[i]*79/4095,BLACK);
//	}
//	tft_update();
//}

u8 distance_checking()
{
	u8 dis_state=0;
	u8 check_state=1;
	u32 readings=0;
	u32 time=get_real_ticks();
	while(check_state) // function for sensing the cloest cylinder
	{
		if((get_real_ticks()-time)>=1000)
		{
			readings = call_ultra_sonic(); 
			time = get_real_ticks();
		}
	
		//slow_line_track();
		
		if((readings > 1000) && (dis_state < 1))
		{
			check_state = 1;
		}
		else if(dis_state < 2)
		{
			check_state = 1;
			dis_state = 1;
		}
		if((readings>1000) && (dis_state >= 1))
		{
			dis_state++;
			
			if(dis_state >=20)
			{
				check_state = 0;
				dis_state=20;
			}
		}
	}
	motor_control(1,0,1);
	motor_control(2,1,1);
}

u8 tubes_checking(uint16_t pin)
{
	//8 left arm
	//7 left grapping
	//6 right arm
	//5  right grapper
	
	if(GPIO_ReadInputDataBit(GPIOA, pin) != 1)
		return 1;
	else
		return 0;
}

//void holder()
//{
//	u32 time = 0;
//	u8 l_correct = 0;
//	u8 r_correct = 0;
//	
//	pneumatic_control(GPIOB,GPIO_Pin_6, 1);
//	time = get_real_ticks();
//	while(get_real_ticks()-time<=20000);
//	pneumatic_control(GPIOB,GPIO_Pin_5, 0);
//	
//	if(tubes_checking(right_pin))
//	{
//		pneumatic_control(GPIOB,GPIO_Pin_6, 0);
//		r_correct++;
//	}
//	else
//	{
//		pneumatic_control(GPIOB,GPIO_Pin_5, 1);
//		pneumatic_control(GPIOB,GPIO_Pin_6, 0);
//	}
//	
//	distance_checking();
//	
//	pneumatic_control(GPIOB,GPIO_Pin_8, 1);
//	time = get_real_ticks();
//	while(get_real_ticks()-time<=20000);
//	pneumatic_control(GPIOB,GPIO_Pin_7, 0);
//	
//	if(tubes_checking(left_pin))
//	{
//		pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//		l_correct++;
//	}
//	else
//	{
//		pneumatic_control(GPIOB,GPIO_Pin_7, 1);
//		pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//	}
//	
//	
//	if((r_correct == 1) && (l_correct == 1))
//	{
//		//code for leaving
//	}
//	else if((r_correct == 1) && (l_correct == 0))
//	{
//		distance_checking();
//		
//		pneumatic_control(GPIOB,GPIO_Pin_8, 1);
//		time = get_real_ticks();
//		while(get_real_ticks()-time<=20000);
//		pneumatic_control(GPIOB,GPIO_Pin_7, 0);
//		
//		if(tubes_checking(left_pin))
//		{
//			pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//			l_correct++;
//			
//			//code for leaving
//		}
//		else
//		{
//			pneumatic_control(GPIOB,GPIO_Pin_7, 1);
//			pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//			
//			distance_checking();
//		
//			pneumatic_control(GPIOB,GPIO_Pin_8, 1);
//			time = get_real_ticks();
//			while(get_real_ticks()-time<=20000);
//			pneumatic_control(GPIOB,GPIO_Pin_7, 0);
//			pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//			
//			//code for leaving
//		}
//	}
//	else if((r_correct == 0) && (l_correct == 1))
//	{
//		//go to the white line
//		if(tubes_checking(right_pin))
//		{
//			pneumatic_control(GPIOB,GPIO_Pin_6, 0);
//			r_correct++;
//			
//			// code for leaving
//		}
//		else
//		{
//			pneumatic_control(GPIOB,GPIO_Pin_5, 1);
//			pneumatic_control(GPIOB,GPIO_Pin_6, 0);
//			
//			//timer go straight
//			pneumatic_control(GPIOB,GPIO_Pin_6, 1);
//			time = get_real_ticks();
//			while(get_real_ticks()-time<=20000);
//			pneumatic_control(GPIOB,GPIO_Pin_5, 0);
//			
//			//code for leaving		
//		}
//	}
//	else
//	{
//		pneumatic_control(GPIOB,GPIO_Pin_6, 1);
//		time = get_real_ticks();
//		while(get_real_ticks()-time<=20000);
//		pneumatic_control(GPIOB,GPIO_Pin_5, 0);
//		
//		pneumatic_control(GPIOB,GPIO_Pin_6, 0);
//		r_correct++;
//		
//		distance_checking();
//		
//		pneumatic_control(GPIOB,GPIO_Pin_8, 1);
//		time = get_real_ticks();
//		while(get_real_ticks()-time<=20000);
//		pneumatic_control(GPIOB,GPIO_Pin_7, 0);

//		pneumatic_control(GPIOB,GPIO_Pin_8, 0);
//		l_correct++;
//		
//		//code for leaving
//	}
//}