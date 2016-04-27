#include "stm32f10x.h"


void Delay(vu32 nCount)
{
	for(; nCount != 0; nCount--);
}
void Delayms(vu32 m)
{
	u32 i;	
	for(; m != 0; m--)	
		for (i=0; i<50000; i++);
}
