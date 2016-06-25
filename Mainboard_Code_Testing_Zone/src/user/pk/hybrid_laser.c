#include "hybrid_laser.h"
#include "pk_arm.h"

#define MOVETIME 1500

int y = 0, x = 0, increment = 0, haha = 0, wagateki = 0, wagamama = 0, savedX=0, savedY=0;
int laserM = 45, laserW = 0, laserB = 0, verticalM = 0, targAngle = 270;
bool fieldDetected = false, targetReached = false, onReach = false;

RETRYSTAGES currRetryStage = INITIAL;

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

bool armPrepped = false;
bool armReturned = false;

int tobiWan = -1;
int prevEcoLaserRead = -1;

void backupFirstPosition(void) {
	int targM = 50, laserTargVal = (robotMode == RED_SIDE ? 405 :370); //340 330
	
	if(get_full_ticks() - timeSinceButtonPressed < 1000) {
		setM(30);
		setBearing(0);
		setW(0);
		addComponent();
		parseWheelbaseValues();
	}
	else {
		double angularSpd = calculateAngularVelocity(robotMode == RED_SIDE ? 90 : 270, 60, 33, 50);
		int delta = 0;
		if(prevEcoLaserRead != -1) delta = get_ls_cal_reading(robotMode == RED_SIDE ? 1 : 0) - prevEcoLaserRead;
		else prevEcoLaserRead = get_ls_cal_reading(robotMode == RED_SIDE ? 1 : 0);
		if(robotMode == RED_SIDE) {
			//Horizontal Vector
			if(get_ls_cal_reading(1) < 1000) {
				targM = targM * (get_ls_cal_reading(1) - 400) / 1000;
				targM = max(2, targM, 10);
			}
			if((get_ls_cal_reading(1) < 550 && robot.position.angle <= 92 && robot.position.angle >= 88 && delta < 400)  ) {//||
				//gpio_read_input(&PE0) == 0) {
				wheelbaseLock();
				//currMode = MANUAL;
				currMode = PIDMODE;
				savedX = get_pos()->x;
				savedY = get_pos()->y;
				queueTargetPoint(savedX, savedY, get_pos()->angle/10, 35, 15, 18, 1500);
				allowArm = true;
				tobiWan = get_ls_cal_reading(1);
			}
			setM(targM);
			setBearing(90 - robot.position.angle);
			setW(angularSpd);
			addComponent();
			
			//Vertical Vector			
			
			int verticalM = Abs((get_ls_cal_reading(2) - laserTargVal) * targM  / (laserTargVal - 200) / 2);
			verticalM = verticalM > (targM / 2) ? targM / 2 : verticalM;
			//verticalM = Abs(verticalM) > 40 ? (verticalM < 0 ? -40 : 40) : verticalM;
			if(robot.position.angle <= 100 && robot.position.angle >= 80) { 
				if(get_ls_cal_reading(2) > laserTargVal + 15) {
					setM(verticalM);
					setBearing(90);
					setW(0);
					addComponent();
				}
				else if(get_ls_cal_reading(2) < laserTargVal - 15){
					setM(verticalM);
					setBearing(270);
					setW(0);
					addComponent();
				}
			}
			
		}
		else if (robotMode == BLUE_SIDE) {
			//Horizontal Vector
			if(get_ls_cal_reading(0) < 1000) {
				targM = targM * (get_ls_cal_reading(0) - 400) / 1000;
				targM = max(2, targM, 10);
			}
			
			if((get_ls_cal_reading(0) < 550 && robot.position.angle <= 272 && robot.position.angle >= 268 && delta < 400)  ) {//||
				//gpio_read_input(&PE0) == 0) {
				wheelbaseLock();
				//currMode = MANUAL;
				currMode = PIDMODE;
				savedX = get_pos()->x;
				savedY = get_pos()->y;
				queueTargetPoint(savedX, savedY, get_pos()->angle/10, 35, 15, 17, 1500);
				allowArm = true;
				tobiWan = get_ls_cal_reading(0);
			}
			
			setM(targM);
			setBearing(270 - robot.position.angle);
			setW(angularSpd);
			addComponent();
			
			//Vertical Vector
			int verticalM = Abs((get_ls_cal_reading(3) - laserTargVal) * targM  / (laserTargVal - 200) / 2);
			verticalM = verticalM > (targM / 2) ? targM / 2 : verticalM;
			//verticalM = Abs(verticalM) > 40 ? (verticalM < 0 ? -40 : 40) : verticalM;
			if(robot.position.angle <= 280 && robot.position.angle >= 260) { 
				if(get_ls_cal_reading(3) > laserTargVal + 15) {
					setM(verticalM);
					setBearing(270);
					setW(0);
					addComponent();
				}
				else if(get_ls_cal_reading(3) < laserTargVal - 15){
					setM(verticalM);
					setBearing(90);
					setW(0);
					addComponent();
				}
			}
		}
		parseWheelbaseValues();
		prevEcoLaserRead = get_ls_cal_reading(robotMode == RED_SIDE ? 1 : 0);
	}
}

void moveToFirstPosition (void) {
	if(!armPrepped) {
			int armError = get_encoder_value(MOTOR8) - 70810;
			if (!(Abs(armError) <= 1000))
			sendArmCommand(armError < 0 ? -40 : 40);
		else if (Abs(armError) <= 1000) {
			sendArmCommand(0);
			armPrepped = true;
			}
		}
	if(get_full_ticks() - timeSinceButtonPressed < MOVETIME) {
		setM(30);
		setBearing(0);
		setW(0);
		addComponent();
		parseWheelbaseValues();
	}
	else{
		int laserTargVal = robotMode == RED_SIDE ? 800 : 800; //1000  //TEST FIELD : 900 : 900
			int horizontalM = 45;
			double angularVelocity;
			if(get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) > (robotMode == RED_SIDE ? 2350 : 2350) && //4400 //TEST FIELD : 4200 : 2300
				get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) < (robotMode == RED_SIDE ? 2700 : 2700) && //4750 // TEST FIELD: 4550 : 2550
				robot.position.angle < 185 && robot.position.angle > 175 && !targetReached){
					targetReached = true;
			}
			else if (targetReached) {
				if(robot.position.angle <= (robotMode == RED_SIDE ? 88 : 268) && robot.position.angle >= (robotMode == RED_SIDE ? 78 : 258) && onReach){
					int armError = get_encoder_value(MOTOR8) - 11000;
					if (!(Abs(armError) <= 1000))
						sendArmCommand(armError < 0 ? -50 : 50);
					else if (Abs(armError) <= 1000) {
						sendArmCommand(0);
						armReturned = true;
					}
					wheelbaseLock();
						if(armReturned) {
							currMode = PIDMODE;
							//approachFirstPosition = false;
							savedX = get_pos()->x;
							savedY = get_pos()->y;
							queueTargetPoint(savedX, savedY, get_pos()->angle/10, 35, 15, 17, 1000);
							//autoPIDMode = true;
							allowArm = true;
							//manualMode = true;
						}
					}
				else if(!onReach) {
					if(get_ls_cal_reading(0) + get_ls_cal_reading(1) < laserTargVal) {
						onReach = true;
					}
					else{
						angularVelocity = getAngleDifference(robot.position.angle, 180 ) * 100 / 180 * -1;
						if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
						else{
							if (angularVelocity > 0) angularVelocity = MAX(30, angularVelocity);
							if (angularVelocity < 0) angularVelocity = MIN(-30, angularVelocity);
						}
						setM(10);
						setBearing(180 - robot.position.angle);
						setW(angularVelocity);
						addComponent();
						parseWheelbaseValues();
					}
				} else{
					int benchmark = Abs(getAngleDifference(robot.position.angle, (robotMode == RED_SIDE ? 83 : 263))) < 10 ?  50 : 90 ;
					int scale = 1;
					if(benchmark == 90) scale = 4;
						angularVelocity = getAngleDifference(robot.position.angle, (robotMode == RED_SIDE ? 83 : 263) ) * 100 * scale / 180 * -1;
					if (Abs(angularVelocity) >= benchmark) angularVelocity = angularVelocity < 0 ? -benchmark : benchmark;
						else{
							if (angularVelocity > 0) angularVelocity = MAX(30, angularVelocity);
							if (angularVelocity < 0) angularVelocity = MIN(-30, angularVelocity);
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
				if(robot.position.angle > 160 && robot.position.angle < 200) {
					angularVelocity = getAngleDifference(robot.position.angle, 180 ) * 35 / 180 * -1;
					if (Abs(angularVelocity) >= 35) angularVelocity = angularVelocity < 0 ? -35 : 35;
					else{
						if (angularVelocity > 0) angularVelocity = MAX(35, angularVelocity);
						if (angularVelocity < 0) angularVelocity = MIN(-35, angularVelocity);
					}
				}
				else {
					angularVelocity = getAngleDifference(robot.position.angle, 180 ) * 80 / 180 * -1;
					if (Abs(angularVelocity) >= 90) angularVelocity = angularVelocity < 0 ? -80 : 100;
					else{
						if (angularVelocity > 0) angularVelocity = MAX(80, angularVelocity);
						if (angularVelocity < 0) angularVelocity = MIN(-80, angularVelocity);
					}
				}
				if(robotMode == RED_SIDE) {
					if(get_ls_cal_reading(2) < 2750) {
						setM(horizontalM);
						setBearing(90 - robot.position.angle); //4750 //TEST FIELD : 4450
					}
					else {
						setM(20);
						setBearing(-90 - robot.position.angle);
					}
				}
				else if(robotMode == BLUE_SIDE) {
					if(get_ls_cal_reading(3) < 2750) {
						setM(horizontalM);
						setBearing(270 - robot.position.angle); //4750 //TEST FIELD : 4450
					}
					else {
						setM(20);
						setBearing(-270 - robot.position.angle);
					}
				}
				setW(angularVelocity);
				addComponent();
				
				//Vertical Vector
				int sum = min(2, get_ls_cal_reading(0), get_ls_cal_reading(1)) * 2;
				int verticalM = sum - laserTargVal;
				int range = laserTargVal - 200;
				int limiter = 55; //Test Field : 55
				if(robot.position.angle > 175 && robot.position.angle < 185){
					verticalM = verticalM  * limiter / range;
					verticalM = min(2, limiter, max(2, -limiter, verticalM));
					setM(verticalM);
					setBearing(180 - robot.position.angle);
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
	int yCoordSystem = robotMode == RED_SIDE ? (semiAuto ? 8200 : 7672) : (semiAuto ? 8200 : 7452);  //Test Field : 7552
	if(fieldDetected) {
		int diff = get_ls_cal_reading(0) - get_ls_cal_reading(1);
		
		if(robotMode == RED_SIDE) offsetDiff = (get_pos()->y < yCoordSystem * 0.7 ? diff: diff-10);
		else if (robotMode == BLUE_SIDE) offsetDiff = (get_pos()->y < yCoordSystem * 0.7 ? diff - 4 : diff+10);
		
		int laserTargVal = robotMode == RED_SIDE ? 600 : 600; //495 //Test Field : 600 600
		int horizontalM = semiAuto ? 60 : 50; //70 //Test Field : 60 50
		
		if(get_pos()->y > 0.86 * yCoordSystem) {
			horizontalM = 30;
		}
		else if(get_pos()->y < yCoordSystem * 0.35 && !semiAuto ) horizontalM = 40; //50 //Test Field 40
		
		laserW = -offsetDiff * (semiAuto ? 1.2 : (get_pos()->y > 0.86 * yCoordSystem ? 1.2 : 1.25)); //1.4 // Test Field 1.2 1.25
		if (laserW > (semiAuto ? 55 : (get_pos()->y > 0.86 * yCoordSystem ? 30 : 40))) { //60 //Test Field 55 30 40
			laserW = semiAuto ? 55 : (get_pos()->y > 0.86 * yCoordSystem ? 30 : 40);
		}
		if (laserW < (semiAuto ? -55 : (get_pos()->y > 0.86 * yCoordSystem ? -30 : -40))) { //-60 //Test Field -55 -30 -40
			laserW = semiAuto ? -55 : (get_pos()->y > 0.86 * yCoordSystem ? -30 : -40); //-60
		}
		
		setM(horizontalM);
		setBearing(robotMode == RED_SIDE ? 270 : 90);
		setW(0);
		addComponent();
		
		/* PK */
		if (get_ls_cal_reading(0) != 1000 && get_ls_cal_reading(1) != 1000) {
		
			int sum = min(2, get_ls_cal_reading(0), get_ls_cal_reading(1)) * 2;
			int verticalM = sum - laserTargVal;
			int range = laserTargVal - 200;
			int cap = semiAuto ? 18 : 16; //20
			
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
					if(get_pos()->y > yCoordSystem * 0.35 && get_pos()->y < yCoordSystem * 0.45) setBrushlessMagnitude(13); //TEST FIELD 18 15
					if(get_pos()->y > yCoordSystem * 0.45 && get_pos()->y < yCoordSystem * 0.55) setBrushlessMagnitude(5); //TEST FIELD 12 10
					if(get_pos()->y > yCoordSystem * 0.55 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(19); //TEST FIELD 15 18
					if(get_pos()->y > yCoordSystem * 0.75) setBrushlessMagnitude(0); //11
				}

				parseWheelbaseValues();
				
					//Finish Conditions
				if(get_pos()->y > yCoordSystem * 0.76 /* && get_ls_cal_reading(1) > 775*/ && !semiAuto) {
						currMode = PIDMODE;
						wheelbaseLock();
						//fieldDetected = false;
						
						//queueTargetPoint(get_pos()->x - 150, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0); //-300 // TEST FIELD -150 0
						//queueTargetPoint(get_pos()->x - 50, get_pos()->y, 175, 2000, 10, -1, 6500); //-200 //TEST FIELD -50 0
						//wagamama = get_pos()->x - 50; //-200
						//wagateki = get_pos()->y; 
					
						int xShift = 240;
						int yShift = 2050;
					
						fieldDetected = false;
						queueTargetPoint(get_pos()->x - xShift - 100, get_pos()->y + 500, get_pos()->angle/10, 400, 240, -1, -1);
						queueTargetPoint(get_pos()->x - xShift - 100, get_pos()->y + yShift + 50, 175, 100, 10, -1, -1);
						queueTargetPoint(get_pos()->x - xShift, get_pos()->y + yShift, 175, 1500, 10, -1, (skipBlowingRiver ? 0 : 5000));
					
						if(skipBlowingRiver) skipBlowingRiver = false;
						
						//queueTargetPoint(get_pos()->x + 150, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0);
						//queueTargetPoint(get_pos()->x + 50, get_pos()->y, 185, 2000, 10, -1, 6000);
						wagamama = get_pos()->x + xShift;
						wagateki = get_pos()->y + yShift;
					}
			}
			else if(robotMode == BLUE_SIDE) {
				if(!semiAuto) {
					if(get_pos()->y > yCoordSystem * 0.35 && get_pos()->y < yCoordSystem * 0.45) setBrushlessMagnitude(13); //TEST FIELD 18
					if(get_pos()->y > yCoordSystem * 0.45 && get_pos()->y < yCoordSystem * 0.55) setBrushlessMagnitude(5); //TEST FIELD 12
					if(get_pos()->y > yCoordSystem * 0.55 && get_pos()->y < yCoordSystem * 0.7) setBrushlessMagnitude(19); //TEST FIELD 15
					if(get_pos()->y > yCoordSystem * 0.75) setBrushlessMagnitude(0);
				}
				parseWheelbaseValues();
				
					//Finish Conditions
					if(get_pos()->y > yCoordSystem * 0.76 /*&& get_ls_cal_reading(0) >800*/ && !semiAuto) {
						currMode = PIDMODE;
						wheelbaseLock();
						
						int xShift = 350;
						int yShift = 2050;
						
						fieldDetected = false;
						queueTargetPoint(get_pos()->x + xShift + 100, get_pos()->y + 500, get_pos()->angle/10, 400, 240, -1, -1);
						queueTargetPoint(get_pos()->x + xShift + 100, get_pos()->y + yShift + 50, 185, 100, 10, -1, -1);
						queueTargetPoint(get_pos()->x + xShift, get_pos()->y + yShift, 185, 1500, 10, -1, (skipBlowingRiver ? 0 : 5000));
						
						if(skipBlowingRiver) skipBlowingRiver = false;
						
						//queueTargetPoint(get_pos()->x + 150, get_pos()->y, get_pos()->angle/10, 100, 50, -1, 0);
						//queueTargetPoint(get_pos()->x + 50, get_pos()->y, 185, 2000, 10, -1, 6000);
						wagamama = get_pos()->x + xShift;
						wagateki = get_pos()->y + yShift;
					}
				}
			}
				else {
					double angularVelocity = getAngleDifference(robot.position.angle, robotMode == RED_SIDE ? 83 : 277) * 50 / 180 * -1;
						if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
						else{
							if (angularVelocity > 0) angularVelocity = MAX(30, angularVelocity);
							if (angularVelocity < 0) angularVelocity = MIN(-30, angularVelocity);
						}
				int sum = get_ls_cal_reading(0) + get_ls_cal_reading(1);
				if(sum <1100 && get_ls_cal_reading(0) < 999 && get_ls_cal_reading(1) < 999){
					fieldDetected = true;
				}
				else{
					setM(45);
					setBearing(robotMode == RED_SIDE ? 266 : 94); //264 //TEST FIELD 266 97
					setW(angularVelocity);
					addComponent();
					parseWheelbaseValues();
				}
			}
		}

		int slowdownDelay = 0;

		
/**
	* @brief Calculates motor values for approaching the wall after blowing
	*/
		
		int redWallCap = 75, blueWallCap = 75;
		
void moveToWall() {
	int armError = get_encoder_value(MOTOR8) - 11000;
	if (!(Abs(armError) <= 1000))
			sendArmCommand(armError < 0 ? -50 : 50);
		else if (Abs(armError) <= 1000) {
			sendArmCommand(0);
		}	
		
	int targetM = robotMode == RED_SIDE ? redWallCap : blueWallCap;
		if(robotMode == RED_SIDE) {
			if(get_ls_cal_reading(2) < 2500 && robot.position.angle < 278 && robot.position.angle > 262){
				targetM = redWallCap * get_ls_cal_reading(2) / 2200;
			}
		}
		else if(robotMode == BLUE_SIDE) {
			if(get_ls_cal_reading(3) < 2500 && robot.position.angle < 98 && robot.position.angle > 82){
				targetM = blueWallCap * get_ls_cal_reading(3) / 2200;
			}
		}
	double angularVelocity = getAngleDifference(robot.position.angle, robotMode == RED_SIDE ? 270 : 90 ) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(30, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-30, angularVelocity);
		}
		setM(targetM);
		if(robotMode == RED_SIDE){
			setBearing(8 - robot.position.angle);
			if(robot.position.angle < 278 && robot.position.angle > 262) setW(angularVelocity);
			else setW(angularVelocity * 2);
		}
		else if(robotMode == BLUE_SIDE) {
			setBearing(352 - robot.position.angle);
			if(robot.position.angle < 98 && robot.position.angle > 82) setW(angularVelocity);
			else setW(angularVelocity * 2);
		}
		//setW(angularVelocity);
		addComponent();
		parseWheelbaseValues();
		if(robotMode == RED_SIDE) {
			if(get_ls_cal_reading(2) < 950 && robot.position.angle < 278 && robot.position.angle > 262){
				currMode = POLELASER;
				climbingState = PREPARATION;
				pneumatics.P1 = true;
				slowdownDelay = get_full_ticks();
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
			if(get_ls_cal_reading(3) < 950 && robot.position.angle < 98 && robot.position.angle > 82){
				currMode = POLELASER;
				climbingState = PREPARATION;
				pneumatics.P1 = true;
				slowdownDelay = get_full_ticks();
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
	x = Abs(((robotMode == RED_SIDE) ? 265: 245) - haha);  //RED : 230 BLUE : 210 //TEST FIELD 295 
	if(robotMode == RED_SIDE) y = 265; // 230 //TEST FIELD 295
	else if(robotMode == BLUE_SIDE) y = 245; //210
	increment = 90 - MIN(90, int_arc_tan2(y,x));
	int mag = 50;
	if(get_full_ticks() - slowdownDelay >= 3500) {
		mag = 20;
	}
	
	if (robotMode == RED_SIDE) {
		double angularVelocity = getAngleDifference(robot.position.angle, 270) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50) angularVelocity = angularVelocity < 0 ? -50 : 50;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(18, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-18, angularVelocity);
		}
		//}
		if (haha > 315) { //245
			setM(mag);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else if (haha < 285) { //215
			setM(mag);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else {
			setM(mag);
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
			if (haha > 280) {
			setM(mag);
			setBearing(180+increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else if (haha < 260) {
			setM(mag);
			setBearing(180-increment);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		} else {
			setM(mag);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
			//sendWheelbaseCommand();
		}
	}
}

int timeSinceFunctionStarted = 0;

void retryToRiverPos (void) {
	int currTime = get_full_ticks();
	int angVel;
	switch (currRetryStage) {
		case INITIAL:
			timeSinceFunctionStarted = currTime;
			currRetryStage = MOVE_FORWARD;
			break;
		case MOVE_FORWARD:
			if(currTime - timeSinceFunctionStarted > 1000) {
				currRetryStage = FIND_GAMEFIELD;
			}
			else{
				setM(40);
				setBearing(0);
				setW(0);
			}
			break;
		case FIND_GAMEFIELD:
			angVel = calculateAngularVelocity(180, 60, 30, 50);
				//Vertical Vectorz
				setM(40);
				setBearing(0 - robot.position.angle);
				if(Abs(getAngleDifference(robot.position.angle, 180)) > 20){
					angVel = 2*angVel;
				}
				setW(angVel);
				addComponent();
			if(robot.position.angle < 180 && robot.position.angle > 170) {
				//HorizontalVectorz
				int horM = 40 * Abs((get_ls_cal_reading(robotMode == RED_SIDE ? 3: 2) - 350)  / (350 - 200) / 2);
				horM = min(2, horM, 50);
				setM(horM);
				if(get_ls_cal_reading(robotMode == RED_SIDE ? 3 : 2) > 350 + 15) setBearing(90);
				else if (get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) < 350 - 15) setBearing(270);
				else setBearing(180);
				setW(0);
				addComponent();
				
				if(get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) < 400) {
					currRetryStage = CHECK_FOR_END;
				}
			}
			break;
		case CHECK_FOR_END:
			setM(30);
			setBearing(0 - robot.position.angle);
			setW(calculateAngularVelocity(180, 40, 20, 40));
			addComponent();
		if(get_ls_cal_reading(robotMode == RED_SIDE ? 2 : 3) > 900) {
				currMode = PIDMODE;
				queueTargetPoint(get_pos()->x, get_pos()->y, (robotMode == RED_SIDE ? 175 : 185), 1500, 10, -1, 4000);
				wagamama = get_pos()->x;
				wagateki = get_pos()->y;
				currRetryStage = STANDBY;
			}
			break;
		case STANDBY:
			break;
	}
}

void retryAutoPath (void) {
	if(get_ls_cal_reading(0) + get_ls_cal_reading(1) < 1200) {
		currMode = LASERPID;
	}
	else{
		setM(40);
		setBearing(0);
		setW(0);
		addComponent();
		parseWheelbaseValues();
	}
}
