#include "imu_math.h"

static s16 last_IMU_reading[6] = {0};
static float Axy[2][2] = {0}; //A is the angle between the force vetor on the xz/yz plane and the z-axis
static float Rest[3] = {0}; //Estimated force vector in x/y/z direction, normalised
static u8 imp = 0; //imu_math_pointer
float ypr[3];

void calc_init(){
	for (u8 i=0; i<3; i++){
		Rest[i] = IMU_Buffer[i]/Abs(IMU_Buffer[i]); //Record the normalised est. force vector
		ypr[i] = 0;
	}
	imp = (imp+1) % 2;
}

void calcIMU(){
	Axy[imp][0] = atan2(Rest[0], Rest[2]); //int_atan2(Rx, Rz); -> Rxz -> on xz plane with z-axis
	Axy[imp][1] = atan2(Rest[1], Rest[2]); //int_atan2(Ry, Rz); -> Ryz -> on yz plane with z-axis
	
	float angle_rate_avg[3] = {0};//Rotation around xyz
	for (u8 i=0; i<3; i++){
		angle_rate_avg[i] = ((float)last_IMU_reading[i+3] + IMU_Buffer[i+3]) / 2.0f / GYRO_GAIN; // Get the average rotational speed
		tft_println("%f", angle_rate_avg[i]);
	}
	
	Axy[(imp+1)%2][0] = Axy[imp][0] + angle_rate_avg[1] * any_loop_diff;  //Generate new angle from last est. reading and gyro
	Axy[(imp+1)%2][1] = Axy[imp][0] + angle_rate_avg[0] * any_loop_diff;  //Generate new angle from last est. reading and gyro
	imp = (imp+1) % 2;
	
	//Value according to the gyro, also normalized
	float Rgyro[3] = {0};
	Rgyro[0] = sin(Axy[imp][0]) / sqrtf((1 + cos(Axy[imp][0])) * (1 + cos(Axy[imp][0])) * (tan(Axy[imp][0])) * (tan(Axy[imp][0])));
	Rgyro[1] = sin(Axy[imp][1]) / sqrtf((1 + cos(Axy[imp][1])) * (1 + cos(Axy[imp][1])) * (tan(Axy[imp][1])) * (tan(Axy[imp][1])));
	Rgyro[2] = (Rest[2]>0.0f?1.0f:-1.0f) * sqrtf(1 - Rgyro[0] * Rgyro[0] - Rgyro[1]*Rgyro[1]);
	
	//Value according to accelerometer
	float Racc[3] = {0};
	for (u8 i=0; i<3; i++){
		Racc[i] = IMU_Buffer[i]; //Record the normalised est. force vector
	}
	
	//Update the estimated force vector
	for (u8 i=0; i<3; i++){
		//Original Formula: Rest[i] = (Racc[i]*w1 + Rgyro[i]*w2) / (w1+w2);
		Rest[i] = (Racc[i] + Rgyro[i] * GYRO_TRUST ) / (1 + GYRO_TRUST);
		ypr[i] = Axy[imp][(i+1)%3];
	}
	
	memcpy(last_IMU_reading, IMU_Buffer, 6 * sizeof(s16));
}
