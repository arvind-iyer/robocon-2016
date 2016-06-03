#include "pk_manual.h"
#include "pk_wheelbase.h"
#include "approx_math.h"
#include "can_xbc_mb.h"
#include "pk_brushless.h"
#include "control.h"
#include "robocon.h"
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
	parseWheelbaseValues();
	//sendWheelbaseCommand();
}

int getTranslationMagnitude() {
	if(!benMode) return (xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY)+xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/12000;
	else return (xbc_get_joy(XBC_JOY_RY)*xbc_get_joy(XBC_JOY_RY)+xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/12000;
}

int getTranslationBearing() {
	if(!benMode) return int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY));
	else return int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_RY));
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

