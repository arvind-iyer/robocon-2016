#include "pk_manual.h"
#include "pk_movement.h"
#include "approx_math.h"
#include "can_xbc_mb.h"
#include "pk_brushless.h"

/**
  * @brief Manual Control of the robot based on the LEFT_JOY and RIGHT_JOY axis values
  */

void manualControl() {
	setM(getTranslationMagnitude());
	setBearing(getTranslationBearing());
	setW(getRotationValue());
	addComponent();
	parseMotorValues();
	sendMotorCommands();
	
}

int getTranslationMagnitude() {
	return (xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY)+xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/10000;
}

int getTranslationBearing() {
	return int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY));
}

int getRotationValue() {
	return xbc_get_joy(XBC_JOY_RX)/10;
}
