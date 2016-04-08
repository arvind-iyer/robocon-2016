#include "imu_math.h"

/************************************************************************
** IMU MATH - STM32F405VG
**
** This library is to keep updating the DCM matrix according to IMU input
** It provides yaw/pitch/roll
** Can also used to rotate any vectors into the body frame
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** April 2016
**
** I find the great thing in this world is not so much where we stand
**     , as in what direction we are moving. ~Oliver Wendell Holmes
*************************************************************************/

float ypr[3];
f_matrix DCM_B;//Use x-y coordinate

void calc_init(){
	for (u8 i=0; i<3; i++){
		ypr[i] = 0.0f;
	}
	matrix_identity(DCM_B);
}

void calcIMU(){
	f_vector acc_vector = {0.0f}; //Vector of acceleration
	f_vector gyr_vector = {0.0f}; //Vector of angular velocity
	f_vector temp_vector = {0.0f};
	
	f_matrix update_matrix;
	matrix_empty(update_matrix);
	
	for (u8 i=0;i<3;i++){
		acc_vector[i] = (float)IMU_Buffer[i]/ACCEL_FACTOR; //First three elements are acceleration
		gyr_vector[i] =  (float)IMU_Buffer[i+3]/GYRO_FACTOR; //Last three elements are angular velocity
	}
	
//	tft_println("%.2f %.2f %.2f", acc_vector[0], acc_vector[1], acc_vector[2]);
//	tft_println("%.2f %.2f %.2f", gyr_vector[0], gyr_vector[1], gyr_vector[2]);
	
	//Find the change in angle by accelerometer
	f_vector d_theta_acc = {0.0f};
	f_vector gravity_vector = {0.0f};
	vector_scale(acc_vector, -1.0f, gravity_vector);
	//Change this to moving weighting to better remove interference from acceleration
	//vector_normalize(gravity_vector, gravity_vector);
	vector_cross(DCM_B[2], vector_minus(gravity_vector, DCM_B[2], temp_vector), d_theta_acc);
	
	tft_println("%.2f %.2f %.2f", d_theta_acc[0], d_theta_acc[1], d_theta_acc[2]);
	
	//Find the change in angle by gyroscope
	f_vector d_theta_gyr = {0.0f};
	vector_scale(gyr_vector, (float)any_loop_diff/1000.0f, d_theta_gyr);
	
	tft_println("%.2f %.2f %.2f", d_theta_gyr[0], d_theta_gyr[1], d_theta_gyr[2]);
	
	//Find the mixed angle
	f_vector d_theta_mix = {0.0f};
	float accel_trust = 1.0f;
	float gyro_trust = 20.0f;
	vector_add(vector_scale(d_theta_acc, accel_trust, d_theta_acc), vector_scale(d_theta_gyr, gyro_trust, d_theta_gyr), temp_vector);
	vector_scale(temp_vector, 1/(accel_trust+gyro_trust), d_theta_mix);
	
	tft_println("%.2f %.2f %.2f", d_theta_mix[0], d_theta_mix[1], d_theta_mix[2]);
	
	//Apply to the update raw matrix
	for (u8 i=0; i<3; i++){
		vector_add(DCM_B[i], vector_cross(d_theta_mix, DCM_B[i], temp_vector), update_matrix[i]);
	}
	
	//Restore orthogonality
	float ortho_error = vector_dot(update_matrix[0], update_matrix[1]) * 0.5f;
	vector_minus(update_matrix[0], vector_scale(update_matrix[1], ortho_error, temp_vector), update_matrix[0]);
	vector_minus(update_matrix[1], vector_scale(update_matrix[0], ortho_error, temp_vector), update_matrix[1]);
	
	//Renormalize the vectors
	vector_normalize(update_matrix[0], DCM_B[0]);
	vector_normalize(update_matrix[1], DCM_B[1]);
	vector_cross(update_matrix[0], update_matrix[1], DCM_B[2]);
	
	//Convert to Euler angles
	ypr[0] = atan2f(DCM_B[0][0], DCM_B[0][1]);
	ypr[1] = asinf(DCM_B[2][0]);
	ypr[2] = atan2f(DCM_B[2][1], DCM_B[2][2]);
}

