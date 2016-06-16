#include "imu_math.h"

s16 abs_diff(s16 minuend, s16 subtrahend){
	s16 diff = minuend - subtrahend;
	while(diff >= 1800){
		diff = 3600 - diff;
	}
	while(diff <= -1800){
		diff = diff + 3600;
	}
	return diff;
}

float fabs_diff(float minuend, float subtrahend){
	float diff = minuend - subtrahend;
	while(diff > 180.0f){
		diff = 360.0f - diff;
        if(diff <= 180.0f){
            return diff;
        }
	}
	while(diff < -180.0f){
		diff = diff + 360.0f;
        if(diff >= 180.0f){
            return diff;
        }
	}
    return diff;
}


float range_remap(float to_be_mapped){
	while(to_be_mapped<0.0f){
		to_be_mapped += 360.0f;
	}
	while(to_be_mapped>360.0f){
		to_be_mapped -= 360.0f;
	}
	return to_be_mapped;
}
