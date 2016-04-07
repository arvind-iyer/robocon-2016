#include "matrix_math.h"

/************************************************************************
** MATRIX MATH - STM32F405VG
**
** This library is for calculation of 3x3 matrix
** especially for a matrix of vectors
** Designed For IMU
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** April 2016
**
** Never send a human to do a machine's job. ~Agent Smith
*************************************************************************/

f_vector* matrix_add(f_matrix a, f_matrix b, f_matrix out_matrix){
	for(int x=0; x<3; x++){
    for(int y=0; y<3; y++){
      out_matrix[x][y] = a[x][y] + b[x][y];
    }
  }
	return out_matrix;
}

f_vector* matrix_multiply(f_matrix a, f_matrix b, f_matrix out_matrix){
  float op[3]; 
  for(int x=0; x<3; x++){
    for(int y=0; y<3; y++){
      for(int w=0; w<3; w++){
       op[w]=a[x][w]*b[w][y];
      }
      out_matrix[x][y] = op[0]+op[1]+op[2];
    }
  }
	return out_matrix;
}

f_vector* matrix_identity(f_matrix matrix){
	matrix[0][0] = 1; 
	matrix[1][0] = 0;
	matrix[2][0] = 0; 
	matrix[0][1] = 0;
	matrix[1][1] = 1;
	matrix[2][1] = 0;
	matrix[0][2] = 0;
	matrix[1][2] = 0;
	matrix[2][2] = 1;
	return matrix;
}

f_vector* matrix_empty(f_matrix matrix){
	for (u8 i=0;i<3;i++){
		for (u8 j=0;j<3;j++){
			matrix[i][j] = 0;
		}
	}
	return matrix;
}
