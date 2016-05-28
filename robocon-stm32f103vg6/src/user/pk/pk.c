#include "pk.h"

void pk_init() {
	//motor_set_acceleration(MOTOR1, 200);
	//motor_set_acceleration(MOTOR2, 200);
	//motor_set_acceleration(MOTOR3, 200);
	pk_wheelbase_init();
	pk_pid_init();
	brushlessStartup();
	hybridPneumaticInit();
	hybridGPIOInit();
	ls_init();
}
