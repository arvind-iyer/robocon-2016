#include "pk_arm.h"
#include "can_motor.h"
#include "hybrid_gpio.h"

void sendArmCommand(int M) {
	if(!fixingArm) motor_set_vel(MOTOR8, M, CLOSE_LOOP);
}
