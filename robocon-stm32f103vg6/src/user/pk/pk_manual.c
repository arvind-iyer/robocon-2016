#include "pk_manual.h"
#include "pk_movement.h"
#include "approx_math.h"
#include "can_xbc_mb.h"
#include "pk_brushless.h"
#include "control.h"
#include "gyro.h"

extern bool rightJoyInUse;

int targetAngle = 0;

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
	return (xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY)+xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/12000;
}

int getTranslationBearing() {
	return int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY));
}

int getRotationValue() {
	return xbc_get_joy(XBC_JOY_RX)/10;
}

void setTargetAngle(int angle) {
	targetAngle = angle;
}

int generateWForTargetAngle(int angle) {
	int magnitude = 0;
	float fmagnitude  = getAngleDifference(angle, get_pos()->angle/10) * 100 / (float)180.0;
	if (fmagnitude >= 2) magnitude = MAX(35, fmagnitude);
	else if (fmagnitude <= -2) magnitude = MIN (-35, fmagnitude);
	else {
		magnitude = fmagnitude;
	}
	return magnitude;
}

