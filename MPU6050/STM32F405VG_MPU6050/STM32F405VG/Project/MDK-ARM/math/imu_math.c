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
static u32 last_ticks = 0;
static s32 rotation_angle[3] = {0};//Scaled by 10
static float DCM_Matrix[3][3];//Use x-y coordinate

void calc_init(){
	for (u8 i=0; i<3; i++){
		rotation_angle[i] = 0;
	}
	
	/* Create a fucking identity matrix
	** 1 0 0 
	** 0 1 0
	** 0 0 1
	**/
	DCM_Matrix[0][0] = 1; 
	DCM_Matrix[1][0] = 0;
	DCM_Matrix[2][0] = 0; 
	DCM_Matrix[0][1] = 0;
	DCM_Matrix[1][1] = 1;
	DCM_Matrix[2][1] = 0;
	DCM_Matrix[0][2] = 0;
	DCM_Matrix[1][2] = 0;
	DCM_Matrix[2][2] = 1;
	
	//The imu uses its own ticks system instead of the main loop one as it requires higher accuracy
	last_ticks = get_ticks();
}

void calcIMU(){
	//R in the following denote the force
	float Racc[3] = {0}; //Vector given by acceleration
	float gyro_raw[3] = {0};
	
	//	u32 interval = get_ticks() - last_ticks; //Calculate its own ticks interval
	//	last_ticks = get_ticks();
	u32 interval = any_loop_diff; //use global loop diff
	
	for (u8 i=0;i<3;i++){
		Racc[i] = (s32)IMU_Buffer[i]/ACCEL_FACTOR; //First three elements are acceleration
		gyro_raw[i] = (s32)IMU_Buffer[i+3]*(s32)interval/1000/GYRO_FACTOR; //Last three elements are angular velocity
	}
	
	float Temp_DCM_Matrix[3][3];
	
	Temp_DCM_Matrix[0][0] = DCM_Matrix[0][0] + DCM_Matrix[1][0]*gyro_raw[2] - DCM_Matrix[2][0]*gyro_raw[1];
	Temp_DCM_Matrix[0][1] = DCM_Matrix[0][1] + DCM_Matrix[1][1]*gyro_raw[2] - DCM_Matrix[2][1]*gyro_raw[1];
	Temp_DCM_Matrix[0][2] = DCM_Matrix[0][2] + DCM_Matrix[1][2]*gyro_raw[2] - DCM_Matrix[2][2]*gyro_raw[1];
	
	Temp_DCM_Matrix[1][0] = -DCM_Matrix[0][0]*gyro_raw[2] + DCM_Matrix[1][0] + DCM_Matrix[2][0]*gyro_raw[0];
	Temp_DCM_Matrix[1][1] = -DCM_Matrix[0][1]*gyro_raw[2] + DCM_Matrix[1][1] + DCM_Matrix[2][1]*gyro_raw[0];
	Temp_DCM_Matrix[1][2] = -DCM_Matrix[0][2]*gyro_raw[2] + DCM_Matrix[1][2] + DCM_Matrix[2][2]*gyro_raw[0];
	
	Temp_DCM_Matrix[2][0] = DCM_Matrix[0][0]*gyro_raw[1] - DCM_Matrix[1][0]*gyro_raw[0] + DCM_Matrix[2][0];
	Temp_DCM_Matrix[2][1] = DCM_Matrix[0][1]*gyro_raw[1] - DCM_Matrix[1][1]*gyro_raw[0] + DCM_Matrix[2][1];
	Temp_DCM_Matrix[2][2] = DCM_Matrix[0][2]*gyro_raw[1] - DCM_Matrix[1][2]*gyro_raw[0] + DCM_Matrix[2][2];
	
	memcpy(DCM_Matrix, Temp_DCM_Matrix, sizeof(float)*9);
	
	for (u8 i=0;i<3;i++){
		tft_println("%.2f %.2f %.2f", DCM_Matrix[0][i], DCM_Matrix[1][i], DCM_Matrix[2][i]);
	}
	
	ypr[2] = atan2(DCM_Matrix[1][2], DCM_Matrix[1][1]);
	ypr[1] = -atan2(DCM_Matrix[2][0], DCM_Matrix[0][0]);
	ypr[0] = asin(DCM_Matrix[1][0]);
}

s16 sqr(s16 num){
	return num*num;
}
