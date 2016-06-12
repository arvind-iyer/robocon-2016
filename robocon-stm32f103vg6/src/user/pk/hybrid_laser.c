#include "hybrid_laser.h"

#define MOVETIME 1500

int y = 0, x = 0, increment = 0, haha = 0, wagateki = 0, wagamama = 0, savedX=0, savedY=0;
int laserM = 45, laserW = 0, laserB = 0, verticalM = 0, targAngle = 270;
bool fieldDetected = false, targetReached = false;

static u8 laser_byte_array[2][100] = {65};
static u8 laser_byte_pointer[2] = {0};
static u32 last_ticks_laser[2] = {0};
static u32 last_init_ticks = 0;
static u8 init_stage = 0;
static bool is_ready = false;

void dual_laser_init(){
	
	last_init_ticks = get_full_ticks();
	init_stage = 0;
}

void dual_laser_init_update(){
	if (get_full_ticks() - last_init_ticks > TICKS_DELAY){
		switch (init_stage){
			case 0:
				//Change data output time
					uart_tx_byte(COM1, 0xFA);
					uart_tx_byte(COM1, 0x04);
					uart_tx_byte(COM1, 0x05);
					uart_tx_byte(COM1, 0x00);
					uart_tx_byte(COM1, 0xFD);
					laser_byte_pointer[COM1] = 0;
				init_stage++;
				break;
				
			case 1:
				//Set freq
					uart_tx_byte(COM1, 0xFA);

					uart_tx_byte(COM1, 0x04);
					uart_tx_byte(COM1, 0x0A);
					uart_tx_byte(COM1, 0x14);
					uart_tx_byte(COM1, 0xE4);
					laser_byte_pointer[COM1] = 0;
				init_stage++;
				break;
				
			case 2:
				//Start laser
					uart_tx_byte(COM1, 0x80);
					uart_tx_byte(COM1, 0x06);
					uart_tx_byte(COM1, 0x03);
					uart_tx_byte(COM1, 0x77);
				init_stage++;
				break;
				
			case 3:
				is_ready = true;
				break;
		}
		last_init_ticks = get_full_ticks();
	}
}

bool is_dual_laser_ready(){
	return is_ready;
}

void LaserHandlerFunction(u8 rx_data) {
	
		laser_byte_array[0][laser_byte_pointer[0]++] = rx_data;
		if (rx_data == 0x80){
			laser_byte_pointer[0] = 1;
			last_ticks_laser[0] = get_full_ticks();
		}
}

//ascii to decimal
inline static u8 a2d(u8 ascii){
	return ascii - 48;
}

//Find the newest data and convert it
s16 get_cheap_laser_dis(u8 id){
	if (!is_ready){
		return 0;
	}
	
	if (laser_byte_array[id][3] == 'E'){
		return -1;
	}else{
		return a2d(laser_byte_array[id][3])*100000 + a2d(laser_byte_array[id][4])*10000 + a2d(laser_byte_array[id][5])*1000
				+ a2d(laser_byte_array[id][7])*100 + a2d(laser_byte_array[id][8])*10 + a2d(laser_byte_array[id][9]);
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
						queueTargetPoint(savedX, savedY, get_pos()->angle/10, 35, 15, 16, 500);
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

/**
	* @brief Laser auto movement part for the uphill part of the game
	*/
int offsetDiff = 0;

void laserPID() {
	int yCoordSystem = robotMode == RED_SIDE ? (semiAuto ? 8200 : 7400) : (semiAuto ? 9800 : 7400);
	if(fieldDetected) {
		int diff = get_ls_cal_reading(0) - get_ls_cal_reading(1);
		
		if(robotMode == RED_SIDE) offsetDiff = (get_pos()->y < yCoordSystem * 0.7 ? diff: diff-10);
		else if (robotMode == BLUE_SIDE) offsetDiff = (get_pos()->y < yCoordSystem * 0.7 ? diff - 4 : diff+10);
		
		int laserTargVal = robotMode == RED_SIDE ? 495 : 495;
		int horizontalM = semiAuto ? 65 : 70;
		
		if(get_pos()->y > 0.86 * yCoordSystem) {
			horizontalM = 30;
		}
		else if(get_pos()->y < yCoordSystem * 0.35 && !semiAuto ) horizontalM = 50;
		
		laserW = -offsetDiff * (semiAuto ? 1.2 : (get_pos()->y > 0.86 * yCoordSystem ? 1.2 : 1.4));
		if (laserW > (semiAuto ? 55 : (get_pos()->y > 0.86 * yCoordSystem ? 30 : 60))) {
			laserW = semiAuto ? 55 : 60;
		}
		if (laserW < (semiAuto ? -55 : (get_pos()->y > 0.86 * yCoordSystem ? -30 : -60))) {
			laserW = semiAuto ? -55 : (get_pos()->y > 0.86 * yCoordSystem ? -30 : -60);
		}
		//laserW = robotMode == RED_SIDE ? -laserW : laserW;
		//laserW = -((int_arc_tan2(diff, 510)-180) * 10 / 180);
		//laserW = (Abs(laserW) > 50 ? (laserW > 0 ? 50 : -50) :laserW);
		//laserB = robotMode == RED_SIDE ? 270 : 90;
		//laserB = 90;
		//if(get_ls_cal_reading(0) + get_ls_cal_reading(1) > 590) laserB = robotMode == RED_SIDE ? 180+45 : 65;
		//else if (get_ls_cal_reading(0) + get_ls_cal_reading(1) < 450) laserB = 	robotMode == RED_SIDE ? 180+115 : 110;
		setM(horizontalM);
		setBearing(robotMode == RED_SIDE ? 270 : 90);
		setW(0);
		addComponent();
		
		/* PK */
		if (get_ls_cal_reading(0) != 1000 && get_ls_cal_reading(1) != 1000) {
		
			int sum = min(2, get_ls_cal_reading(0), get_ls_cal_reading(1)) * 2;
			int verticalM = sum - laserTargVal;
			int range = laserTargVal - 200;
			int cap = semiAuto ? 18 : 21;
			
			verticalM = verticalM  * (cap) / range;
			verticalM = min(2, cap, max(2, -cap, verticalM));
			verticalM = verticalM > 0 ? verticalM : verticalM*2;
			
			setM(verticalM);
			setBearing(0);
			setW(laserW);
			addComponent();
		}
		
		//Blowing speeds
			if(robotMode == RED_SIDE) {
				if(!semiAuto){
					if(get_pos()->y > yCoordSystem * 0.4 && get_pos()->y < yCoordSystem * 0.55) setBrushlessMagnitude(10);
					if(get_pos()->y > yCoordSystem * 0.55 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(9);
					if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(0);
				}

				parseWheelbaseValues();
				
					//Finish Conditions
				if(get_pos()->y > yCoordSystem /* && get_ls_cal_reading(1) > 775*/ && !semiAuto) {
						wheelbaseLock();
						autoModeLaser = false;
						//manualMode = true;
						manualMode = false;
						autoPIDMode = true;
						fieldDetected = false;
						
						queueTargetPoint(get_pos()->x - 300, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0);
						queueTargetPoint(get_pos()->x - 200, get_pos()->y, 175, 2000, 10, -1, 6500);
						wagateki = get_pos()->y;
						wagamama = get_pos()->x - 200;
						
					}
			}
			else if(robotMode == BLUE_SIDE) {
				if(!semiAuto) {
					if(get_pos()->y > yCoordSystem * 0.5 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(14);
					if(get_pos()->y > yCoordSystem * 0.7 && get_pos()->y < yCoordSystem * 0.81) setBrushlessMagnitude(16);
				}
				parseWheelbaseValues();
				
					//Finish Conditions
					if(get_pos()->y > yCoordSystem /*&& get_ls_cal_reading(0) >800*/ && !semiAuto) {
						wheelbaseLock();
						autoModeLaser = false;
						//manualMode = true;
						manualMode = false;
						autoPIDMode = true;
						fieldDetected = false;
						
		
						queueTargetPoint(get_pos()->x + 300, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0);
						queueTargetPoint(get_pos()->x + 250, get_pos()->y, 185, 35, 5, -1, 6500);
						wagamama = get_pos()->x + 250;
						wagateki = get_pos()->y;
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
					setM(45);
					setBearing(robotMode == RED_SIDE ? 264 : 97);
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
