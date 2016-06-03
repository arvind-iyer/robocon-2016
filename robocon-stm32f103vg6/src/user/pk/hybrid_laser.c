#include "hybrid_laser.h"

int yCoordSystem = 7000;

int y = 0, x = 0, increment = 0, haha = 0;
int laserM = 45, laserW = 0, laserB = 0, verticalM = 0, targAngle = 270;

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
			//sendWheelbaseCommand();
		} else if (haha < 245) {
			setM(40);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else {
			setM(40);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
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
			//sendWheelbaseCommand();
		} else if (haha < 2) {
			setM(40);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else {
			setM(40);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		}
	}
}

void laserPID() {
	int diff = get_ls_cal_reading(0) - get_ls_cal_reading(1);
	//int offsetDiff = (get_pos()->y < yCoordSystem * 0.578 ? diff : diff+50);
	int offsetDiff = diff + 25;
	int laserTargVal = 520;
	int horizontalM = 30;
	laserW = offsetDiff < -35	 ? 30 : (offsetDiff > 35 ? -30 : 0);
	//laserW = -((int_arc_tan2(diff, 510)-180) * 10 / 180);
	//laserW = (Abs(laserW) > 50 ? (laserW > 0 ? 50 : -50) :laserW);
  //laserB = robotMode == RED_SIDE ? 270 : 90;
	//laserB = 90;
	//if(get_ls_cal_reading(0) + get_ls_cal_reading(1) > 590) laserB = robotMode == RED_SIDE ? 180+45 : 65;
	//else if (get_ls_cal_reading(0) + get_ls_cal_reading(1) < 450) laserB = robotMode == RED_SIDE ? 180+115 : 110;
	setM(horizontalM);
	setBearing(90);
	setW(laserW);
	addComponent();
	
	int sum = get_ls_cal_reading(0) + get_ls_cal_reading(1);
	int verticalM = sum - laserTargVal;
	int range = laserTargVal - 200;
	
//	if (Abs(laserM) < 50) {
//		laserM = 0;
//	}
	verticalM = verticalM  * horizontalM / range;
	verticalM = min(2, 30, max(2, -30, verticalM));
	setM(verticalM);
	setBearing(0);
	setW(0);
	addComponent();
	
	//Blowing speeds
	//if(get_pos()->y > yCoordSystem * 0.5 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(21);
	//if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(14);
	
	parseWheelbaseValues();
	//sendWheelbaseCommand();
	
	if(get_ls_cal_reading(0) > 800){
	//Finish Conditions
	//if(get_pos()->y > yCoordSystem) {
					wheelbaseLock();
					autoModeLaser = false;
					//manualMode = true;
					currStage = STAGE4;
//					manualMode = false;
//					autoPIDMode = true;
//					queueTargetPoint(get_pos()->x + 250, 7505, 185, 35.0, 5.0, -1, 6500);
//					//queueTargetPoint(0, 11000, 200, 500, 200, -1, 0);//lost point
//					queueTargetPoint(50, 8508, 200, 500, 200, -1, 0);
//					queueTargetPoint(-103, 12046, 88, 800, 200, -1, 0);
//					queueTargetPoint(-1200, 12810, 86, 800, 200, -1, 0);
	}
}

void laserCallbacks(STAGES stage) {
	int laserR = get_ls_cal_reading(0), laserL = get_ls_cal_reading(1), angle = get_pos()->angle/10;
	double angularVelocity = getAngleDifference(angle, targAngle) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(18, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-18, angularVelocity);
		}
	
	switch (stage) {
		case STAGE1:
			targAngle = 270;
			if(laserR > 1000 && laserL > 1000 && (angle >= 268 && angle/10 <=272)) {
				setM(45);
				setBearing(0);
				setW(0);
				addComponent();
				parseWheelbaseValues();
			}
			else if(angle < 268 || angle > 272){
				setM(45);
				setBearing(0);
				setW(angularVelocity);
				addComponent();
				parseWheelbaseValues();
			}
			else {
				wheelbaseLock();
				currStage = STAGE2;
			}
			break;
		case STAGE2:
			if(laserR > 200 && laserL > 200) {
				setM(30);
				setBearing(90);
				setW(0);
				addComponent();
				parseWheelbaseValues();
			}
			else {
				wheelbaseLock();
				currStage = STAGE3;
			}
			break;
		case STAGE3:
				autoModeLaser = true;
				currStage = LISTEN;
			break;
		case STAGE4:
			targAngle = 185;
			if(laserL <350 && angle >= 184 && angle <= 186) {
				setM(15);
				setBearing(270);
				setW(0);
				addComponent();
				parseWheelbaseValues();
			}
			else if(angle < 184 || angle > 186) {
				setM(15);
				setBearing(270);
				setW(angularVelocity);
			}
			break;
		case STAGE5:
			break;
		default:
			break;
	}
}
