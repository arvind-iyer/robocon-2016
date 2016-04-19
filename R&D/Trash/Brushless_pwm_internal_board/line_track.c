#include "line_track.h"



u16 white_threshold1,white_threshold2;
u16 black_threshold1,black_threshold2;
u8 mid[2][2] = {{0,127},{0,127}};
u8 past_index=0;

enum states
{
	NORMAL,
	DEC,
	DECL,
	DECR,
	LS,
	RS
};

u8 line_track_init(u8 state)
{
	if(state==1)
	{
		mid[1][0]=mid[0][0]=64;
		mid[1][0]=mid[0][1]=0;
		for(u8 i=LEFT;i<RIGHT;i++)
			white_threshold1+=(linear_ccd_buffer1[i]/(RIGHT-LEFT));
		tft_clear();
		tft_prints(0,0,"CCD1 White Set");
		tft_update();
		return 0;
	}
	else if(state==2)
	{
		for(u8 i=LEFT;i<RIGHT;i++)
			black_threshold1+=(linear_ccd_buffer1[i]/(RIGHT-LEFT));
		int temp=white_threshold1;
		white_threshold1=temp*2/3+black_threshold1/3;
		black_threshold1=(temp+black_threshold1)/2;
		tft_clear();
		tft_prints(0,0,"CCD1 Black Set");
		tft_update();
		return 0;
	}
	else if(state==3)
	{
		for(u8 i=LEFT;i<RIGHT;i++)
			white_threshold2+=(linear_ccd_buffer2[i]/(RIGHT-LEFT));
		tft_clear();
		tft_prints(0,0,"CCD2 White Set");
		tft_update();
		return 0;
	}
	else 
	{
		for(u8 i=LEFT;i<RIGHT;i++)
			black_threshold2+=(linear_ccd_buffer2[i]/(RIGHT-LEFT));
		int temp=white_threshold2;
		white_threshold2=temp*2/3+black_threshold2/3;
		black_threshold2=(temp+black_threshold2)/2;
		tft_clear();
		tft_prints(0,2, "B: %d", black_threshold2);
		tft_prints(0,3, "W: %d", white_threshold2);		
		tft_prints(0,0,"CCD2 Black Set");
		tft_update();
		return 1;
	}
}

u8 stop=0;
u8 turn_count=0;
u8 line_track(u8 track_state)
{
	u8 ccd1_mids[2]={0}, ccd2_mids[2]={0};	//mid-pt of white lines detected (at most 2 white lines so size=2)			
	u8 i,index=0;
	u16 time=get_real_ticks();
	
	linear_ccd_read();
		
	for(i=LEFT;i<RIGHT;i++)
	{
		if(linear_ccd_buffer1[i]>white_threshold1)
		{
			ccd1_mids[index]=i;
			for(;i<RIGHT&&linear_ccd_buffer1[i]>black_threshold1;i++);
			ccd1_mids[index]=(ccd1_mids[index]+i)/2;
			index++;
		}
	}
	index=0;
	for(i=LEFT;i<RIGHT;i++)
	{
		if(linear_ccd_buffer2[i]>white_threshold2)
		{
			ccd2_mids[index]=i;
			for(;i<RIGHT&&linear_ccd_buffer2[i]>black_threshold2;i++);
			//if(ccd2_mids[index]<LEFT+WIDTH && i>RIGHT-WIDTH)
			ccd2_mids[index]=(ccd2_mids[index]+i)/2;
			index++;
			
		}
	}
	
	
	
	u8 mid1 = MAX(MIN(mid[0][0],mid[0][1]),MIN(MAX(mid[0][0],mid[0][1]),ccd1_mids[0])); 
	u8 mid2 = MAX(MIN(mid[1][0],mid[1][1]),MIN(MAX(mid[1][0],mid[1][1]),ccd2_mids[0]));
	
	if(ccd2_mids[0]==0)
	{
		motor_control(1,1,mid[1][0]>=64?200:100);
		motor_control(2,0,mid[1][0]>64?100:200);
	}
	else
	{
		motor_control(1,1,ccd2_mids[0]>=64?200:100);
		motor_control(2,0,ccd2_mids[0]>64?100:200);
		mid[1][0]=ccd2_mids[0];
	}
	
	//Display
	tft_clear();
	tft_prints(0,0,"TOP");
	tft_prints(0,1,"Tracking...");
	tft_prints(0,2,"MID[]: %d %d", mid[1][0],mid[1][1]);
	tft_prints(0,3,"MID2: %d",ccd2_mids[0]);
	
	for(u8 i=0;i<128;i++)
	{
		tft_put_pixel(i,159-white_threshold2*79/4095,YELLOW);
		tft_put_pixel(i,159-black_threshold2*79/4095,BLUE);
		tft_put_pixel(i,159-linear_ccd_buffer1[i]*79/4095,GREEN);
		tft_put_pixel(i,159-linear_ccd_buffer2[i]*79/4095,WHITE);
	}
	for(u8 i=0;i<128;i++)
	{
		tft_put_pixel(i,159-linear_ccd_buffer1[i]*79/4095,BLACK);
		tft_put_pixel(i,159-linear_ccd_buffer2[i]*79/4095,BLACK);
	}
	tft_update();
	
	return track_state;
}

u8 read()
{
	u8 left=0;
	u8 right=0;
	linear_ccd_read();
	for(u8 i=LEFT;i<RIGHT;i++)
	{
		if(linear_ccd_buffer1[i]>white_threshold1)
			for(u8 j=i;j<RIGHT;j++)
				if(linear_ccd_buffer1[j]<black_threshold1 && j-i>10)
					left=1;
		if(linear_ccd_buffer2[i]>white_threshold2)
			for(u8 j=i;j<RIGHT;j++)
				if(linear_ccd_buffer2[j]<black_threshold2 && j-i>10)
					right=1;
	}
	return right&&left;
}

u8 line()
{
	u8 first;
	linear_ccd_read();
	for(u8 i=LEFT;i<RIGHT;i++)
	{
		if(linear_ccd_buffer2[i]>white_threshold2)
		{
			first=i;
			for(;i<RIGHT && linear_ccd_buffer2[i]>black_threshold2;i++);
			if((i+first)/2>54 && (i+first/2<74))
				return 1;
		}	
	}
	return 0;
}

void halt()
{
	motor_control(1,1,1);
	motor_control(2,0,1);
	u16 time=get_real_ticks();
	while(get_real_ticks()-time<500);
}

void turn(u8 dir)
{
	u16 time = get_real_ticks();
	while(!line())
	{
		motor_control(1,dir,200);
		motor_control(2,dir,200);
	}
}

void go(u16 duration)
{
	u16 time = get_real_ticks();
	while(get_real_ticks()-time<duration || read())
	{
		motor_control(1,1,200);
		motor_control(2,0,180);
	}
}

void arc(u16 duration, u8 dir)
{
	u16 time = get_real_ticks();
	while(get_real_ticks()-time<duration)
	{
		motor_control(1,1,dir?150:200);
		motor_control(2,0,dir?200:150);
	}
}

void crappy_line_track(u8 track_state)
{
	u8 left = 0;
	u8 right =1;
	go(1500);
	go(2000);
	halt();
	turn(right);
	halt();
	go(2000);
	halt();
	turn(right);
	halt();
	go(2000);
	halt();
	turn(right);
	halt();
	go(2000);
	halt();
	turn(left);
	halt();
	arc(4000,right);
	halt();
	turn(left);
	halt();
	go(2000);
	halt();
	turn(left);
	halt();
	go(2000);
	halt();
	turn(left);
	halt();
	go(2000);
	halt();
	turn(right);
	halt();
	go(2000);
	halt();
	turn(left);
	halt();
	go(2000);
	halt();
}
