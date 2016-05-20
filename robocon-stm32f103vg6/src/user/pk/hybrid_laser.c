#include "hybrid_laser.h"
int y = 0, x = 0, increment = 0, haha = 0;



void enterPole() {
	int angleDiff = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE) ? 270 : 1);
	haha = int_cos(angleDiff * 10) * get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1)/10000;
	x = Abs(((robotMode == RED_SIDE) ? 255: 0) - haha);
	if(robotMode == RED_SIDE) {
		y = 255;
	}
	increment = 90 - MIN(90, int_arc_tan2(y,x));
	
	if (robotMode == RED_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 270) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 20) angularVelocity = angularVelocity < 0 ? -20 : 20;
		if (angularVelocity > 0) angularVelocity = MAX(5, angularVelocity);
		if (angularVelocity < 0) angularVelocity = MIN(-5, angularVelocity);
		//}
		if (haha > 265) {
			setM(60);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else if (haha < 245) {
			setM(60);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else {
			setM(80);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		}
	}
	else if(robotMode == BLUE_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 90) / 180 * 100 * 2.9;
		if (angularVelocity >= 1) angularVelocity = MAX(50, angularVelocity);
		else if(angularVelocity <= -1) angularVelocity = MIN(-50, angularVelocity);
		if (haha > 230) {
			setM(haha / 225 * 5);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else if (haha < 190) {
			setM(1 + 	(haha / 195) * 5);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else {
			setM(45);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		}
	}
}
