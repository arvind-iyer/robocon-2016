#include "matrix_math.h"

/************************************************************************
** MATRIX MATH - STM32F405VG
**
** This library is for calculation of 3x3 matrix
** especially for a matrix of vectors
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
