#include "imu_math.h"

float abs_diff(float minuend, float subtrahend){
	float diff = minuend - subtrahend;
	return diff>180.0f ? diff-360.0f : (diff<-180.0f ? -(diff+360.0f) : diff);
}

