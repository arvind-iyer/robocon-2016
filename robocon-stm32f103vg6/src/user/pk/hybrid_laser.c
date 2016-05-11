#include "hybrid_laser.h"

void enterPole() {
	int haha = get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1);
	if (robotMode == RED_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 270) / 180 * 100 * 2.9;
		if (angularVelocity >= 1) angularVelocity = MAX(50, angularVelocity);
		if (haha > 290) {
			int temp = 180 - MIN(90, (int) (haha - 272.5) / 45 * 40);
			setM(haha * 10 / 285);
			setBearing(temp);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else if (haha < 245) {
			int temp = 180 - MAX(-90, (int) (haha - 272.5) / 45 * 40);
			setM(1 + 	(haha * 10 / 245));
			setBearing(temp);
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
	else if(robotMode == BLUE_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 90) / 180 * 100 * 2.9;
		if (angularVelocity >= 1) angularVelocity = MAX(50, angularVelocity);
		if (haha > 230) {
			int temp = 180 + MIN(90, (int) (haha - 210) / 45 * 40);
			setM(haha / 225 * 5);
			setBearing(temp);
			setW(angularVelocity);
			addComponent();
			parseMotorValues();
			sendMotorCommands();
		} else if (haha < 190) {
			int temp = 180 + MAX(-90, (int) (haha - 210) / 45 * 40);
			setM(1 + 	(haha / 195) * 5);
			setBearing(temp);
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
