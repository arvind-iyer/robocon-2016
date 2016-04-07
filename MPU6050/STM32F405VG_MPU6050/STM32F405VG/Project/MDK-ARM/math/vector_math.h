#ifndef _VECTOR_MATH_H
#define _VECTOR_MATH_H

#include "stm32f4xx.h"
#include <math.h>
#include "quick_math.h"

typedef float f_vector[3]; //Define f_vector as a array of three floats

float vector_dot(f_vector v1, f_vector v2);
float* vector_cross(f_vector v1, f_vector v2, f_vector out_vector);
float* vector_scale(f_vector v1, float scalar, f_vector out_vector);
float* vector_add(f_vector v1, f_vector v2, f_vector out_vector);
float* vector_minus(f_vector v1, f_vector v2, f_vector out_vector);
float* vector_normalize(f_vector v1, f_vector out_vector);

#endif
