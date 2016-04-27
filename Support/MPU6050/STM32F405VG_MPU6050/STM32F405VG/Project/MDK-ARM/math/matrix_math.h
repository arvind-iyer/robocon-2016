#ifndef _MATRIX_MATH_H
#define _MATRIX_MATH_H

#include "stm32f4xx.h"
#include <math.h>
#include "vector_math.h"
#include "quick_math.h"

typedef f_vector f_matrix[3]; //Define f_vector as a array of three vectors

f_vector* matrix_add(f_matrix a, f_matrix b, f_matrix out_matrix);
f_vector* matrix_multiply(f_matrix a, f_matrix b, f_matrix out_matrix);
f_vector* matrix_identity(f_matrix matrix);
f_vector* matrix_empty(f_matrix matrix);

#endif
