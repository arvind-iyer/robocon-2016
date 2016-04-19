#include "stm32f10x.h"
#include <stdint.h>

//IMPORTANT! ALL OF THEM ARE SCALED TO A FACTOR OF 100 TO INCREASE ACCURACY
//These three should be modified according to need.
#define Kp 1750 //Decrease if shaking //
#define Ki 0 //Increase if constantly out of line //
#define Kd 3000 //Increase if cannot handle sharp turn //
#define	DECAY_FACTOR 20//Increase if car still turn too much even in black line //
//END OF SCALING

#define D_ARRAYSIZE 2u //Increase to avoid sudden shake due to noise 
#define PRIMARY_WEIGHT 5u
#define SECONDARY_WEIGHT 3u

void pid_init(u8 initPos);
s16 getAdjustment(u8 primaryReading, u8 secondaryReading, s16*, s16*);
