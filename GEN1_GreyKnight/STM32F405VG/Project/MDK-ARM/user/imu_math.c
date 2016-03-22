#include "imu_math.h"

float abs_diff(float minuend, float subtrahend){
	float diff = minuend - subtrahend;
	while(diff >= 180.0f){
		diff = 360.0f - diff;
	}
	while(diff <= -180.0f){
		diff = diff + 360.0f;
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