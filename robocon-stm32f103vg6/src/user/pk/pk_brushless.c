#include "pk_brushless.h"
#include "servo.h"
#include "can_xbc_mb.h"

int brushlessMag = 0;

/**
  * @brief Startup the brushless, to be called with other inits
  */

void brushlessStartup() {
	int timer;
	timer=get_seconds();
	while (get_seconds()-timer <= 1) {
		servo_control(SERVO4, 450);
	}
	servo_control(SERVO4, 460);
}

int getBrushlessMagnitude() {
	return brushlessMag;
}

/**
  * @brief Controlling the brushless fans, servo ports can be changed accordingly
	* @param m: Magnitude of the fan: 0 - 100
  */

void setBrushlessMagnitude(int m) {
	brushlessMag = m;
	servo_control(SERVO4, MIN_PWM + (brushlessMag * (MAX_PWM-MIN_PWM) / 100));
}
