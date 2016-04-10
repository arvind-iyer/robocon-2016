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
static f_vector acc_corr_i = {0};

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
		acc_vector[i] = (float)IMU_Buffer[i]/ACCEL_FACTOR; //First three elements are acceleration, Unit is g
		gyr_vector[i] =  (float)IMU_Buffer[i+3]/GYRO_FACTOR; //Last three elements are angular velocity, Unit is deg/s
	}
	
//	tft_println("%.2f %.2f %.2f", acc_vector[0], acc_vector[1], acc_vector[2]);
//	tft_println("%.2f %.2f %.2f", gyr_vector[0], gyr_vector[1], gyr_vector[2]);
	
	//Find the rate of change of radian by accelerometer, using PI controller
	f_vector d_omega_acc = {0.0f};
	float acc_length = vector_len(acc_vector);
	float acc_weight;

	//If the acceleration is too small/too large, the robot is under high external acceleration --> ignore
	if (acc_length<0.3f || acc_length>1.7f){
		acc_weight = 0;
	}else{
		acc_weight = fabs(1.0f - acc_length);
	}
	
	f_vector acc_err_vector;
	f_vector acc_corr_p;
	
	vector_scale(vector_cross(DCM_B[2], acc_vector, temp_vector), -1.0f*acc_weight, acc_err_vector);
	vector_scale(acc_err_vector, IMU_ACCEL_P, acc_corr_p);
	vector_add(vector_scale(acc_err_vector, IMU_ACCEL_I, temp_vector), acc_corr_i, acc_corr_i);
	vector_add(acc_corr_p, acc_corr_i, d_omega_acc);
	
//	tft_println("%f", vector_len(temp_vector));
//	tft_println("%f", acc_weight);
//	tft_println("%f", vector_len(acc_err_vector));
	
	//Find the rate of change of radian by gyroscope -- RADIAN <-- Very important, wasted a few days in this
	f_vector d_omega_gyr = {0.0f};
	vector_scale(gyr_vector, pi/180.0f, d_omega_gyr);
	
	//tft_println("%.2f %.2f %.2f", d_theta_gyr[0], d_theta_gyr[1], d_theta_gyr[2]);
	
	//Find the mixed rate of change of radian
	f_vector d_theta_mix = {0.0f};
	float accel_trust = 2.0f;
	float gyro_trust = 0.0f;
	
	//Apply dt and weighted average
	vector_add(vector_scale(d_omega_acc, accel_trust, d_omega_acc), vector_scale(d_omega_gyr, gyro_trust, d_omega_gyr), temp_vector);
	vector_scale(temp_vector, ((float)any_loop_diff/1000.0f)/(accel_trust+gyro_trust), d_theta_mix);
	
	//tft_println("%f", vector_len(d_omega_gyr));
	//tft_println("%.2f %.2f %.2f", d_theta_mix[0], d_theta_mix[1], d_theta_mix[2]);
	
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
	vector_cross(DCM_B[0], DCM_B[1], DCM_B[2]);
	
	//Convert to Euler angles
	ypr[0] = atan2f(DCM_B[0][1], DCM_B[0][0])*180.0f/pi; //RADIAN <-- Very important, wasted a few days in this
	ypr[1] = -asinf(DCM_B[0][2])*180.0f/pi;	//RADIAN <-- Very important, wasted a few days in this
	ypr[2] = atan2f(DCM_B[1][2], DCM_B[2][2])*180.0f/pi; //RADIAN <-- Very important, wasted a few days in this
}

