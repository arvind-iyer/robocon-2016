#include "hybrid_laser.h"

int yCoordSystem = 7000;

int y = 0, x = 0, increment = 0, haha = 0;
int laserM = 45, laserW = 0, laserB = 0;

void enterPole() {
	int angleDiff = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE) ? 270 : 90);
	haha = int_cos(angleDiff * 10) * get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 2)/10000;
	x = Abs(((robotMode == RED_SIDE) ? 255: 210) - haha);
	if(robotMode == RED_SIDE) y = 255;
	else if(robotMode == BLUE_SIDE) y = 210;
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
			if (haha > 220) {
			setM(40);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			sendWheelbaseCommand();
		} else if (haha < 2) {
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

void laserPID() {
	int diff = get_ls_cal_reading(0) - get_ls_cal_reading(1);
	int offsetDiff = (get_pos()->y < yCoordSystem * 0.578 ? diff : diff+100);
	laserW = offsetDiff < -35	 ? 50 : (offsetDiff > 35 ? -50 : 0);
	//laserW = -((int_arc_tan2(diff, 510)-180) * 10 / 180);
	//laserW = (Abs(laserW) > 50 ? (laserW > 0 ? 50 : -50) :laserW);
  //laserB = robotMode == RED_SIDE ? 270 : 90;
	//laserB = 90;
	//if(get_ls_cal_reading(0) + get_ls_cal_reading(1) > 590) laserB = robotMode == RED_SIDE ? 180+45 : 65;
	//else if (get_ls_cal_reading(0) + get_ls_cal_reading(1) < 450) laserB = robotMode == RED_SIDE ? 180+115 : 110;
	setM(60);
	setBearing(90);
	setW(laserW);
	addComponent();
	
	int sum = get_ls_cal_reading(0) + get_ls_cal_reading(1);
	int laserM = sum - 520;
//	if (Abs(laserM) < 50) {
//		laserM = 0;
//	}
	laserM = laserM * 3 / 16;
	laserM = min(2, 40, max(2, -40, laserM));
	setM(laserM);
	setBearing(0);
	setW(0);
	addComponent();
	
	//Blowing speeds
	if(get_pos()->y > yCoordSystem * 0.5 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(18);
	if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(12);
	
	parseWheelbaseValues();
	sendWheelbaseCommand();
	
	//if(get_ls_cal_reading(0) > 800){
	//Finish Conditions
	if(get_pos()->y > yCoordSystem) {
					wheelbaseLock();
					autoModeLaser = false;
					manualMode = false;
					autoPIDMode = true;
					queueTargetPoint(get_pos()->x + 250, 7505, 185, 35.0, 5.0, -1, 6500);
					queueTargetPoint(0, 11000, 200, 500, 200, -1, 0);//lost point
					queueTargetPoint(50, 8508, 200, 500, 200, -1, 0);
					queueTargetPoint(-103, 12046, 88, 800, 200, -1, 0);
					queueTargetPoint(-1200, 12810, 86, 800, 200, -1, 0);
	}
}
