#include "vector_math.h"

/************************************************************************
** VECTOR MATH - STM32F405VG
**
** This library is for calculation of 3-dimensional vectors
** Designed For IMU
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** April 2016
**
** Our scientific power has outrun our spiritual power.
**     We have guided missiles and misguided men. ~Martin Luther King Jr.
*************************************************************************/

float vector_dot(f_vector v1, f_vector v2){
	float result = 0;
	for (u8 i=0;i<3;i++){
		result += v1[i]*v2[i];
	}
	return result;
}

float* vector_cross(f_vector v1, f_vector v2, f_vector out_vector){
  out_vector[0]= v1[1]*v2[2] - v1[2]*v2[1];
  out_vector[1]= v1[2]*v2[0] - v1[0]*v2[2];
  out_vector[2]= v1[0]*v2[1] - v1[1]*v2[0];
	return out_vector;
}

float* vector_scale(f_vector v1, float scalar, f_vector out_vector){
	for (u8 i=0;i<3;i++){
		out_vector[i] = v1[i] * scalar;
	}
	return out_vector;
}

float* vector_add(f_vector v1, f_vector v2, f_vector out_vector){
	for (u8 i=0;i<3;i++){
		out_vector[i] = v1[i] + v2[i];
	}
	return out_vector;
}

float* vector_minus(f_vector v1, f_vector v2, f_vector out_vector){
	for (u8 i=0;i<3;i++){
		out_vector[i] = v1[i] - v2[i];
	}
	return out_vector;
}

float* vector_normalize(f_vector v1, f_vector out_vector){
	float length = sqrtf(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	out_vector[0] = v1[0]/length;
	out_vector[1] = v1[1]/length;
	out_vector[2] = v1[2]/length;
	return out_vector;
}
