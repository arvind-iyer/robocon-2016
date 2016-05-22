#include "pk_arm.h"
#include "can_motor.h"

void sendArmCommand(int M) {
	motor_set_vel(MOTOR8, M, CLOSE_LOOP);
}
