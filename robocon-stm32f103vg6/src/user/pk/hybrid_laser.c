#include "hybrid_laser.h"
int y = 0, x = 0, increment = 0, haha = 0;



void enterPole() {
	int angleDiff = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE) ? 270 : 90);
	haha = int_cos(angleDiff * 10) * get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1)/10000;
	x = Abs(((robotMode == RED_SIDE) ? 255: 255) - haha);
	if(robotMode == RED_SIDE) y = 255;
	else if(robotMode == BLUE_SIDE) y = 255;
	increment = 90 - MIN(90, int_arc_tan2(y,x));
	
	if (robotMode == RED_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 270) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(18, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-18, angularVelocity);
		}
		//}
		if (haha > 265) {
			setM(40);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		} else if (haha < 245) {
			setM(40);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		} else {
			setM(40);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		}
	}
	else if(robotMode == BLUE_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 90) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(18, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-18, angularVelocity);
		}
			if (haha > 265) {
			setM(40);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		} else if (haha < 245) {
			setM(40);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		} else {
			setM(40);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		}
	}
}
