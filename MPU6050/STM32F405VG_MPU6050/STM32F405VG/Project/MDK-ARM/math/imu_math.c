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
bool imu_ignoring_accel = false, imu_gyro_saturated = false;
f_vector acc_vector = {0.0f}; //Vector of acceleration
f_vector gyr_vector = {0.0f}; //Vector of angular velocity

void print_dcm_matrix(){
	tft_println("%.2f %.2f %.2f", DCM_B[0][0], DCM_B[0][1], DCM_B[0][2]);
	tft_println("%.2f %.2f %.2f", DCM_B[1][0], DCM_B[1][1], DCM_B[1][2]);
	tft_println("%.2f %.2f %.2f", DCM_B[2][0], DCM_B[2][1], DCM_B[2][2]);
}

void imu_load_data(){
	getRawAccelGyro();  
	
	acc_vector[0] = IMU_Buffer[0]>0?(float)IMU_Buffer[0]/ACCEL_1G_POS_X:(float)IMU_Buffer[0]/ACCEL_1G_NEG_X;
	acc_vector[1] = IMU_Buffer[1]>0?(float)IMU_Buffer[1]/ACCEL_1G_POS_Y:(float)IMU_Buffer[1]/ACCEL_1G_NEG_Y;
	acc_vector[2] = IMU_Buffer[2]>0?(float)IMU_Buffer[2]/ACCEL_1G_POS_Z:(float)IMU_Buffer[2]/ACCEL_1G_NEG_Z;
	
	gyr_vector[0] = ((float)IMU_Buffer[3]-GYRO_X_STATIC_OFFSET)/GYRO_FACTOR;
	gyr_vector[1] = ((float)IMU_Buffer[4]-GYRO_Y_STATIC_OFFSET)/GYRO_FACTOR;
	gyr_vector[2] = ((float)IMU_Buffer[5]-GYRO_Z_STATIC_OFFSET)/GYRO_FACTOR;
	
	for (u8 i=0;i<3;i++){
		if (IMU_Buffer[i+3]>32767 || IMU_Buffer[i+3]<-32767){
			imu_gyro_saturated = true;
			return; //Exit the function
		}
	}
	imu_gyro_saturated = false;
}

void calc_init(){
	for (u8 i=0; i<3; i++){
		ypr[i] = 0.0f;
	}
	f_vector temp_vector = {0.0f};
	imu_load_data();
	matrix_identity(DCM_B);
	
	vector_scale(acc_vector, 1.0f, acc_vector);
	vector_cross(DCM_B[2], acc_vector, temp_vector);
	vector_add(DCM_B[0], temp_vector, DCM_B[0]);
	vector_add(DCM_B[1], temp_vector, DCM_B[1]);
	vector_add(DCM_B[2], temp_vector, DCM_B[2]);
	
	#if YAW_USING_ABS_GYRO
		ypr[0] = 0.0f;
	#endif
}

void calcIMU(){
	f_vector temp_vector = {0.0f};
	
	f_matrix update_matrix;
	matrix_empty(update_matrix);
	
	imu_load_data();

	//Find the rate of change of radian by accelerometer, using PI controller
	f_vector d_omega_acc = {0.0f};
	float acc_length = vector_len(acc_vector);
	float acc_weight;

	//If the acceleration is too small/too large, the robot is under high external acceleration --> ignore
	if (acc_length<0.6f || acc_length>1.4f){
		acc_weight = 0;
		imu_ignoring_accel = true;
	}else{
		acc_weight = 2.0f*fabs(1.0f - acc_length);
		imu_ignoring_accel = false;
	}
	
	f_vector acc_err_vector;
	f_vector acc_corr_p;
	
	vector_scale(vector_cross(DCM_B[2], acc_vector, temp_vector), -1.0f*acc_weight, acc_err_vector);
	vector_scale(acc_err_vector, IMU_ACCEL_P, acc_corr_p);
	vector_add(vector_scale(acc_err_vector, IMU_ACCEL_I, temp_vector), acc_corr_i, acc_corr_i);
	vector_add(acc_corr_p, acc_corr_i, d_omega_acc);
	
	//Find the rate of change of radian by gyroscope -- RADIAN <-- Very important, wasted a few days in this
	f_vector d_omega_gyr = {0.0f};
	vector_scale(gyr_vector, pi/180.0f, d_omega_gyr);
	
	//Find the mixed rate of change of radian
	f_vector d_theta_mix = {0.0f};
	float accel_trust = 1.0f;
	float gyro_trust = 20.0f;
	
	//Apply dt and weighted average
	vector_add(vector_scale(d_omega_acc, accel_trust, d_omega_acc), vector_scale(d_omega_gyr, gyro_trust, d_omega_gyr), temp_vector);
	vector_scale(temp_vector, ((float)any_loop_diff/1000.0f)/(accel_trust+gyro_trust), d_theta_mix);

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
	#if YAW_USING_ABS_GYRO
		ypr[0] += gyr_vector[2]*any_loop_diff/1000.0f;
	#else
		ypr[0] = atan2f(DCM_B[1][0], DCM_B[0][0])*180.0f/pi; //RADIAN <-- Very important, wasted a few days in this
	#endif
	ypr[1] = -asinf(DCM_B[2][0])*180.0f/pi;	//RADIAN <-- Very important, wasted a few days in this
	ypr[2] = atan2f(DCM_B[2][1], DCM_B[2][2])*180.0f/pi; //RADIAN <-- Very important, wasted a few days in this
}

void calibration_mode_loop(){
	s16 max_pos_acc[3] = {0};
	s16 max_neg_acc[3] = {0};
	float avg_gyro[3] = {0};
	s16 acc_now[3] = {0};
	s16 gyro_now[3] = {0};
	s32 sum_of_gyro[3] = {0};
	u32 gyro_count = 0;
	while(1){
		tft_clear();
		tft_println("%d", get_ticks());
		getRawAccelGyro();  
		for (u8 i=0;i<3;i++){
			acc_now[i] = IMU_Buffer[i];
			gyro_now[i] =  IMU_Buffer[i+3];
		}
		for (u8 i=0;i<3;i++){
			if (acc_now[i]>max_pos_acc[i]){
				max_pos_acc[i] = acc_now[i];
			}else if (acc_now[i]<max_neg_acc[i]){
				max_neg_acc[i] = acc_now[i];
			}
			sum_of_gyro[i] += gyro_now[i];
			gyro_count++;
			avg_gyro[i] = (float)sum_of_gyro[i]/gyro_count;
		}
		for (u8 i=0;i<3;i++){
			tft_println("%d %d %d", max_pos_acc[i], max_neg_acc[i], acc_now[i]);
		}
		for (u8 i=0;i<3;i++){
			tft_println("%f %d", avg_gyro[i], gyro_now[i]);
		}
		tft_update();
	}
}
