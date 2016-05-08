#include "pk_movement.h"
#include "approx_math.h"
#include "can_motor.h"

Motors motor;

int xComponent;
int yComponent;
int rotation;

int M;
int bearing;
int W;

/**
  * @brief Initialize the pk_movement robot variables for manual control
  */

void pk_movement_init() {
	motor_set_acceleration(MOTOR1, 200);
	motor_set_acceleration(MOTOR2, 200);
	motor_set_acceleration(MOTOR3, 200);
	xComponent = 0;
	yComponent = 0;
	rotation = 0;
	motor.M1 = 0;
	motor.M2 = 0;
	motor.M3 = 0;
	motor.M4 = 0;
	motor.M5 = 0;
	motor.M6 = 0;
	motor.M7 = 0;
	motor.M8 = 0;
	M = 0;
	bearing = 0;
	W = 0;
}

/**
  * @brief Setting the target magnitude
	* @param _M : the magnitude you want the robot to go at
  */

void setM(int _M) {
	M = _M;
}

/**
  * @brief Setting the target bearing
	* @param _Bearing: the bearing you want the robot to go to. Range: 0 - 360
  */

void setBearing(int _Bearing) {
	bearing = _Bearing % 360;
}

/**
  * @brief Setting the rotational speed of the hybrid
	* @param _W the magnitude of the rotational speed
  */

void setW(int _W) {
	W = _W;
}

void addComponent() {
	double x = M * MAX_VEL * int_sin(bearing*10)/10000 / 100;
	double y = M * MAX_VEL* int_cos(bearing*10)/10000 / 100;
	xComponent = xComponent + x;
	yComponent = yComponent + y;
	rotation = rotation + W;
	M = 0;
	bearing = 0;
	W = 0;
}

/**
  * @brief Resolving motor values for the three wheelbase motors depending on xComponent, yComponent and rotation
  */

void parseMotorValues() {
	float M1 = (-rotation - xComponent * 2) / 3;
	float M2 = ((-rotation * 0.5774 + xComponent * 0.5774 - yComponent) / 1.732);
	float M3 = -rotation - M1 - M2;
	motor.M1 = M1;
	motor.M2 = M2;
	motor.M3 = M3;
	xComponent = 0;
	yComponent = 0;
	rotation = 0;
	
}

void sendWheelBaseMotorCommands (int M1, int M2, int M3) {
	motor.M1 = M1;
	motor.M2 = M2;
	motor.M3 = M3;
	motor_set_vel(MOTOR1, motor.M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, motor.M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, motor.M3, CLOSE_LOOP);
}

/**
  * @brief Sending the motor commands to its respective motors
  */

void sendMotorCommands() {
	motor_set_vel(MOTOR1, motor.M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, motor.M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, motor.M3, CLOSE_LOOP);
	motor_set_vel(MOTOR4, motor.M4, OPEN_LOOP);
	motor_set_vel(MOTOR5, motor.M5, OPEN_LOOP);
	motor_set_vel(MOTOR6, motor.M6, OPEN_LOOP);
	motor_set_vel(MOTOR7, motor.M7, OPEN_LOOP);
	motor_set_vel(MOTOR8, motor.M8, CLOSE_LOOP);
}

/**
  * @brief Sending the motor commands to the respective climbing motors
	* @param Motor values for each motor 4,5,6,7
  */

void sendClimbCommands(int M4, int M5, int M6, int M7) {
	motor.M4 = M4;
	motor.M5 = M5;
	motor.M6 = M6;
	motor.M7 = M7;
	motor_set_vel(MOTOR4, motor.M4, OPEN_LOOP);
	motor_set_vel(MOTOR5, motor.M5, OPEN_LOOP);
	motor_set_vel(MOTOR6, -motor.M6, OPEN_LOOP);
	motor_set_vel(MOTOR7, motor.M7, OPEN_LOOP);
}

/**
  * @brief Sending the arm command to the arm motor, also updates the M8 motor value
* @param integer M8 : magnitude of the motor speed
   */

void sendArmCommand(int M8) {
	motor.M8 = M8;
	motor_set_vel(MOTOR8, motor.M8, CLOSE_LOOP);
}

/**
  * @brief Getter for motor values
	* @retval The Motor structure
  */

Motors getMotorValues() {
	return motor;
}
