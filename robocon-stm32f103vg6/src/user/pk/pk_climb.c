#include "pk_climb.h"
#include "can_motor.h"

void sendClimbCommand(int M) {
	motor_set_vel(MOTOR4, -M, OPEN_LOOP);
	motor_set_vel(MOTOR5, M, OPEN_LOOP);
	motor_set_vel(MOTOR6, -M, OPEN_LOOP);
	motor_set_vel(MOTOR7, M, OPEN_LOOP);
}
