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
static s32 rotation_angle[3] = {0};//Scaled by 10
static f_matrix DCM_B;//Use x-y coordinate

void calc_init(){
	for (u8 i=0; i<3; i++){
		rotation_angle[i] = 0;
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
	
	//Find the change in angle by accelerometer
	f_vector d_theta_acc = {0.0f};
	f_vector gravity_vector = {0.0f};
	vector_scale(acc_vector, -1, gravity_vector);
	vector_normalize(gravity_vector, gravity_vector);
	vector_cross(DCM_B[2], vector_minus(gravity_vector, DCM_B[2], temp_vector), d_theta_acc);
	
	//Find the change in angle by gyroscope
	f_vector d_theta_gyr = {0.0f};
	vector_scale(acc_vector, (float)any_loop_diff/1000.0f, d_theta_gyr);
	
	//Find the mixed angle
	f_vector d_theta_mix = {0.0f};
	float accel_trust = 1.0f;
	float gyro_trust = 20.0f;
	vector_add(vector_scale(d_theta_acc, accel_trust, d_theta_acc), vector_scale(d_theta_gyr, gyro_trust, d_theta_gyr), temp_vector);
	vector_scale(temp_vector, 1/(accel_trust+gyro_trust), d_theta_mix);
	
	//Apply to the update matrix
	for (u8 i=0; i<3; i++){
		vector_add(DCM_B[i], vector_cross(d_theta_mix, DCM_B[i], temp_vector), update_matrix[i]);
	}
	
	//Update for orthogonality
	f_vector ortho_error = {0.0f};
	vector_scale(vector_add(DCM_B[0], DCM_B[1], temp_vector), 0.5f, ortho_error);
	
}

s16 sqr(s16 num){
	return num*num;
}
