#include "imu_math.h"

static float Awz[2] = {0}; //A is the angle between the force vetor on the xz/yz plane and the z-axis
float Rest[3] = {0}; //Estimated force vector in x/y/z direction, normalised
float ypr[3];
static u32 last_ticks = 0;

void calc_init(){
	float Racc[3] = {0};
	for (u8 i=0; i<3; i++){
		Racc[i] = (float)IMU_Buffer[i]; //Record the normalised est. force vector
	}
	
	//normalise_vector_float(Racc);
	memcpy(Rest, Racc, sizeof(float) * 3); //Record the normalised est. force vector
	
	for (u8 i=0; i<3; i++){
		ypr[i] = 0;
	}
	//The imu uses its own ticks system instead of the main loop one as it requires higher accuracy
	last_ticks = get_ticks();
}

void calcIMU(){
	//R in the following denote the force
	float Racc[3] = {0}; //Vector given by acceleration
	float RGyro[3] = {0}; //Vector given by gyro
	float gyro_raw[3] = {0};
	
	//	u32 interval = get_ticks() - last_ticks; //Calculate its own ticks interval
	//	last_ticks = get_ticks();
	u32 interval = any_loop_diff; //use global loop diff
	
	for (u8 i=0;i<3;i++){
		Racc[i] = (float)IMU_Buffer[i]/ACCEL_FACTOR; //First three elements are acceleration
		gyro_raw[i] = (float)IMU_Buffer[i+3]*interval/1000.0f/GYRO_FACTOR; //Last three elements are angular velocity
	}
	
	normalise_vector_float(Racc);//Normalise the acceleration vector(first 3 in the array)
	
	if (fabsf(Rest[2] < 0.1f)){
		//When Rz is too small, error induced to Axz and Ayz is significant
		//Therefore we skip it here
		for (u8 i=0; i<3; i++){
			RGyro[i] = Rest[i];
		}
	}else{
		//Get the angles between projection of R on the zx/zy plane with \
		//readings from the gyro and last estimated reading
		for (u8 i=0; i<2; i++){
			Awz[i] = atan2(Rest[i], Rest[2]) * 180.0f / pi; //Convert from radian to degree
			Awz[i] += gyro_raw[i];
		}
		
		//Detemine the sign of RzGyro based on the quadrant of Axz
		s8 sign_of_RzGyro = (cos(Awz[0]*pi/180.0f)>=0) ? 1: -1;
		for (u8 i=0; i<2; i++){
			RGyro[i] = sin(Awz[i]*pi/180.0f);
			RGyro[i] /= sqrtf(1 + sqr(cos(Awz[i]*pi/180.0f)) * sqr(tan(Awz[1-i]*pi/180.0f)));
		}
		RGyro[2] = sign_of_RzGyro * sqrt(1 - sqr(RGyro[0]) - sqr(RGyro[1]));
		//normalise_vector_float(RGyro);
	}
	
	//Complementary filter
	for (u8 i=0; i<2; i++){
		Rest[i] = (Racc[i] + RGyro[i] * GYRO_TRUST) / (1 + GYRO_TRUST);
	}
	
	normalise_vector_float(Rest); //Normalise the vector again
	
	ypr[0] += gyro_raw[2];
	ypr[1] = atan2(Rest[0], sqrt(sqr(Rest[1]) + sqr(Rest[2])))*180.0f/pi; //roll
	ypr[2] = atan2(-Rest[1], Rest[2])*180.0f/pi; //pitch
}

float sqr(float num){
	return num*num;
}

/**
** Normalise a vector into length of 1
** @param An array of 3 float to be normalised
*/
void normalise_vector_float(float* v){
	float R=sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= R;
	v[1] /= R;
	v[2] /= R;
}
