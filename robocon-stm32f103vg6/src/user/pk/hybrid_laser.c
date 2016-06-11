#include "hybrid_laser.h"

#define MOVETIME 1500

int y = 0, x = 0, increment = 0, haha = 0, wagateki = 0, savedX=0, savedY=0;
int laserM = 45, laserW = 0, laserB = 0, verticalM = 0, targAngle = 270;
bool fieldDetected = false, targetReached = false;

/**
	* @brief Calculates values for the motor required to enter the pole
	*/

void enterPole() {
	int angleDiff = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE) ? 270 : 90);
	haha = int_cos(angleDiff * 10) * get_ls_cal_reading((robotMode == RED_SIDE) ? 2 : 3)/10000;
	x = Abs(((robotMode == RED_SIDE) ? 250: 210) - haha);
	if(robotMode == RED_SIDE) y = 250;
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
		} else if (haha < 235) {
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
			if (haha > 265) {
			setM(40);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else if (haha < 235) {
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

/**
	* @brief Laser auto movement part for the uphill part of the game
	*/

void laserPID() {
	int yCoordSystem = robotMode == RED_SIDE ? 7682 : 7682;
	if(fieldDetected) {
		int diff = get_ls_cal_reading(0) - get_ls_cal_reading(1);
		int offsetDiff = (get_pos()->y < yCoordSystem * 0.7 ? diff : diff+10);
		//int offsetDiff = diff;
		int laserTargVal = robotMode == RED_SIDE ? 495 : 495;
		int horizontalM = 50;
		if(get_pos()->y > 5500) horizontalM = 30;
		else if(get_pos()->y < yCoordSystem * 0.37) horizontalM = 30;
		/* this is yours Kristian */
		//laserW = offsetDiff < -35	 ? 30 : (offsetDiff > 35 ? -30 : 0);
		/* PK */
		laserW = -offsetDiff * 1.2;
		if (laserW > 30) {
			laserW = 30;
		}
		if (laserW < -30) {
			laserW = -30;
		}
		//laserW = robotMode == RED_SIDE ? -laserW : laserW;
		//laserW = -((int_arc_tan2(diff, 510)-180) * 10 / 180);
		//laserW = (Abs(laserW) > 50 ? (laserW > 0 ? 50 : -50) :laserW);
		//laserB = robotMode == RED_SIDE ? 270 : 90;
		//laserB = 90;
		//if(get_ls_cal_reading(0) + get_ls_cal_reading(1) > 590) laserB = robotMode == RED_SIDE ? 180+45 : 65;
		//else if (get_ls_cal_reading(0) + get_ls_cal_reading(1) < 450) laserB = robotMode == RED_SIDE ? 180+115 : 110;
		setM(horizontalM);
		setBearing(robotMode == RED_SIDE ? 270 : 90);
		setW(0);
		addComponent();
		
		/* PK */
		if (get_ls_cal_reading(0) != 1000 && get_ls_cal_reading(1) != 1000) {
		
			int sum = min(2, get_ls_cal_reading(0), get_ls_cal_reading(1)) * 2;
			int verticalM = sum - laserTargVal;
			int range = laserTargVal - 200;
			
			verticalM = verticalM  * (horizontalM) / range;
			verticalM = min(2, 30, max(2, -30, verticalM));
			setM(verticalM);
			setBearing(0);
			setW(laserW);
			addComponent();
		}
		
		//Blowing speeds
			if(robotMode == RED_SIDE) {
				if(!semiAuto){
					if(get_pos()->y > yCoordSystem * 0.4 && get_pos()->y < yCoordSystem * 0.6) setBrushlessMagnitude(14);
					if(get_pos()->y > yCoordSystem * 0.6 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(24);
					if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(0);
				}

				parseWheelbaseValues();

				//if(get_ls_cal_reading(0) > 800){
				
					//Finish Conditions
				if(get_pos()->y > 4000 && get_ls_cal_reading(1) > 800 && !semiAuto) {
						wheelbaseLock();
						autoModeLaser = false;
						//manualMode = true;
						//currStage = STAGE4;
						manualMode = false;
						autoPIDMode = true;
						fieldDetected = false;
						
						queueTargetPoint(get_pos()->x - 300, get_pos()->y-300, get_pos()->angle/10, 100, 50, -1, 0);
						queueTargetPoint(get_pos()->x - 250, get_pos()->y-300, 175, 35, 5, -1, 6500);
						wagateki = get_pos()->y - 300;
						
						//queueTargetPoint(0, 11000, 200, 500, 200, -1, 0);//lost point
						//queueTargetPoint(50, 8508, 200, 500, 200, -1, 0);
						//queueTargetPoint(-103, 12046, 88, 800, 200, -1, 0);
						//queueTargetPoint(-1200, 12810, 86, 800, 200, -1, 0);
					}
			}
			else if(robotMode == BLUE_SIDE) {
				if(!semiAuto) {
					if(get_pos()->y > yCoordSystem * 0.5 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(21);
					if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(14);
				}
				parseWheelbaseValues();

				//if(get_ls_cal_reading(0) > 800){
				
					//Finish Conditions
					if(get_pos()->y > 4000 && get_ls_cal_reading(0) >800 && !semiAuto) {
						wheelbaseLock();
						autoModeLaser = false;
						//manualMode = true;
						//currStage = STAGE4;
						manualMode = false;
						autoPIDMode = true;
						fieldDetected = false;
						
		
						queueTargetPoint(get_pos()->x + 300, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0);
						queueTargetPoint(get_pos()->x + 250, get_pos()->y, 185, 35, 5, -1, 6500);
						wagateki = get_pos()->y;
						
						//queueTargetPoint(0, 11000, 200, 500, 200, -1, 0);//lost point
						//queueTargetPoint(50, 8508, 200, 500, 200, -1, 0);
						//queueTargetPoint(-103, 12046, 88, 800, 200, -1, 0);
						//queueTargetPoint(-1200, 12810, 86, 800, 200, -1, 0);
					}
				}
			}
				else {
					double angularVelocity = getAngleDifference(robot.position.angle, robotMode == RED_SIDE ? 83 : 277) * 50 / 180 * -1;
						if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
						else{
							if (angularVelocity > 0) angularVelocity = MAX(20, angularVelocity);
							if (angularVelocity < 0) angularVelocity = MIN(-20, angularVelocity);
						}
				int sum = get_ls_cal_reading(0) + get_ls_cal_reading(1);
				if(sum <1100 && get_ls_cal_reading(0) < 999 && get_ls_cal_reading(1) < 999){
					fieldDetected = true;
				}
				else{
					setM(30);
					setBearing(robotMode == RED_SIDE ? 263 : 97);
					setW(angularVelocity);
					addComponent();
					parseWheelbaseValues();
				}
			}
		}

/**
	* @brief Calculates motor values for approaching the wall after blowing
	*/
		
void moveToWall() {
	int targetM = robotMode == RED_SIDE ? 70 : 70;
	double angularVelocity = getAngleDifference(robot.position.angle, robotMode == RED_SIDE ? 270 : 90 ) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(30, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-30, angularVelocity);
		}
		
		setM(targetM);
		if(robotMode == RED_SIDE) setBearing(10 - robot.position.angle);
		else if(robotMode == BLUE_SIDE) setBearing(350 - robot.position.angle);
		setW(angularVelocity);
		addComponent();
		parseWheelbaseValues();
		if(robotMode == RED_SIDE) {
			if(get_ls_cal_reading(2) < 850 && robot.position.angle < 278 && robot.position.angle > 262){
				laserAuto = true;
				pneumatics.P1 = true;
				wallApproach = false;
				if(pneumatics.P1 != true) {
					pneumatics.P1 = true;
					pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
				}
				if(pneumatics.P3 != false) {
					pneumatics.P3 = false;
					pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
				}
			}
		}
		else if(robotMode == BLUE_SIDE){
			if(get_ls_cal_reading(3) < 850 && robot.position.angle < 98 && robot.position.angle > 82){
				laserAuto = true;
				pneumatics.P1 = true;
				wallApproach = false;
				if(pneumatics.P1 != true) {
					pneumatics.P1 = true;
					pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
				}
				if(pneumatics.P3 != false) {
					pneumatics.P3 = false;
					pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
				}
			}
		}
}

void moveToFirstPosition (void) {
	if(get_full_ticks() - timeSinceButtonPressed < MOVETIME) {
		setM(30);
		setBearing(0);
		setW(0);
		addComponent();
		parseWheelbaseValues();
	}
	else{
			int laserTargVal = 1000;
			int horizontalM = 45;
			double angularVelocity;
			if(get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) > (robotMode == RED_SIDE ? 4600 : 2300) && 
				robot.position.angle < 185 && robot.position.angle > 175 && !targetReached){
					targetReached = true;
			}
			else if (targetReached) {
				if(robot.position.angle <= (robotMode == RED_SIDE ? 85 : 265) && robot.position.angle >= (robotMode == RED_SIDE ? 80 : 260)){
						wheelbaseLock();
						approachFirstPosition = false;
						savedX = get_pos()->x;
						savedY = get_pos()->y;
						queueTargetPoint(savedX, savedY, get_pos()->angle/10, 35, 15, 15, 500);
						autoPIDMode = true;
						allowArm = true;
						//manualMode = true;
					}
					else{
						angularVelocity = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE ? 83 : 263) ) * 80 / 180 * -1;
						if (Abs(angularVelocity) >= 70) angularVelocity = angularVelocity < 0 ? -70 : 70;
						else{
							if (angularVelocity > 0) angularVelocity = MAX(70, angularVelocity);
							if (angularVelocity < 0) angularVelocity = MIN(-70, angularVelocity);
						}
						setM(0);
						setBearing(0);
						setW(angularVelocity);
						addComponent();
						parseWheelbaseValues();
					}
			}
			else{
				//Horizontal Vector
				if(robot.position.angle > 165 && robot.position.angle < 195) {
					angularVelocity = getAngleDifference(robot.position.angle, 180 ) * 40 / 180 * -1;
					if (Abs(angularVelocity) >= 60) angularVelocity = angularVelocity < 0 ? -50 : 50;
					else{
						if (angularVelocity > 0) angularVelocity = MAX(35, angularVelocity);
						if (angularVelocity < 0) angularVelocity = MIN(-35, angularVelocity);
					}
				}
				else {
					angularVelocity = getAngleDifference(robot.position.angle, 180 ) * 90 / 180 * -1;
					if (Abs(angularVelocity) >= 90) angularVelocity = angularVelocity < 0 ? -90 : 90;
					else{
						if (angularVelocity > 0) angularVelocity = MAX(90, angularVelocity);
						if (angularVelocity < 0) angularVelocity = MIN(-90, angularVelocity);
					}
				}
				setM(horizontalM);
				if(robotMode == RED_SIDE) setBearing(90 - robot.position.angle);
				else if(robotMode == BLUE_SIDE) setBearing (270 - robot.position.angle);
				setW(angularVelocity);
				addComponent();
				
				//Vertical Vector
				int sum = min(2, get_ls_cal_reading(0), get_ls_cal_reading(1)) * 2;
				int verticalM = sum - laserTargVal;
				int range = laserTargVal - 200;
				if(robot.position.angle > 175 && robot.position.angle < 185){
					verticalM = verticalM  * horizontalM / range;
					verticalM = min(2, 30, max(2, -30, verticalM));
					setM(verticalM);
					setBearing(0);
					setW(0);
					addComponent();
				}
				parseWheelbaseValues();	
			}
	}
}


////PROTOTYPE FUNCTION UNTESTED DO NOT USE
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
			if(laserR > 950 && laserL > 950 && (angle >= 268 && angle/10 <=272)) {
				setM(45);
				setBearing(0);
				setW(0);
				addComponent();
				parseWheelbaseValues();
			}
			else if(angle < 268 || angle > 272){
				setM(45);
				setBearing(0);
				setW(-angularVelocity);
				addComponent();
				parseWheelbaseValues();
			}
			else {
				wheelbaseLock();
				currStage = STAGE2;
			}
			break;
		case STAGE2:
			if(laserR > 270 && laserL > 270) {
				setM(30);
				setBearing(90);
				setW(0);
				addComponent();
				parseWheelbaseValues();
			}
			else {
				wheelbaseLock();
				//currStage = STAGE3;
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
