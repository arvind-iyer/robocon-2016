#include "imu_math.h"

s32 Rest[3] = {0};

void calc_init(){
	for (u8 i=0; i<3; i++){
		Rest[i] = IMU_Buffer[i];
	}
}

void calcIMU(){
	
}