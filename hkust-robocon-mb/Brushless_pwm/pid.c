#include "pid.h"
u32 PastError[D_ARRAYSIZE];
u32 PointerToError = 0;
u32 MidPos = 0;
s16 Integral = 0;

void pid_init(u8 initPos)
{
	for (u32 i=0;i<D_ARRAYSIZE;i++)
		PastError[i] = initPos;
	MidPos = initPos; 
	PointerToError = 0;
	Integral = 0;
}

s8 getPastAverage()
{
	s16 sum = 0;
	for (u32 i=0;i<D_ARRAYSIZE;i++)
		sum += PastError[i];
	return sum/D_ARRAYSIZE;
}

s16 getAdjustment(u8 primaryReading, u8 secondaryReading, s16 *P, s16 *D)
{

	u8 TOTAL_WEIGHT = PRIMARY_WEIGHT + SECONDARY_WEIGHT;
	s16 Error = ((primaryReading-MidPos)*PRIMARY_WEIGHT + 
					(secondaryReading-MidPos)*SECONDARY_WEIGHT) / TOTAL_WEIGHT;//Find weighted error

	Integral = (Integral * DECAY_FACTOR / 100) + Error;
	
	s16 adjustment = 
		(Error * Kp + (Error - getPastAverage()) * Kd + Integral * Ki) /100;

	*P = Error*Kp/100;
	*D = Kd*(Error - getPastAverage())/100;
	
	//Refresh the past error array
	PastError[PointerToError % D_ARRAYSIZE] = Error;
	PointerToError = (PointerToError + 1) % D_ARRAYSIZE;

	return (adjustment);
}
