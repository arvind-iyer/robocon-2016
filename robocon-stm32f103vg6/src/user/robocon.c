#include "robocon.h"
#include "pk/pk.h"

extern int dispCurrentDistance;
extern int dispCurrentBearing;
extern int dispW;
extern int timediff;
extern bool laserAuto;

//#define DEBUG_MODE
#define RED_SIDE 0
#define BLUE_SIDE 1

bool robotMode = false;
bool manualMode = true;
bool autoPIDMode = false;
bool autoModeLaser = false;
bool rightJoyInUse = false;
bool laserAuto = false;

//Variables for allowing the update of the button
bool _allowUpdate = false, allowDPadUpdate = false, allowArm = false, allowArmUpdate = false, allowModeUpdate = false, allowPIDUpdate = false;
bool allowAngleUpdate = true, allowAutoLazers = false;
bool climbing = false;

void robocon_main(void)
{	
		tft_init(0, BLACK, WHITE, SKY_BLUE);
		pk_init();
	
	while (1) {
		if(get_full_ticks()%25 == 0){
			reset();
			button_update();
			_updateScreen();
			controllerInputUpdate();
			if (manualMode)manualControl();
			hybridPneumaticControl();
			limitSwitchCheck();
			if(allowArm) {
				armIr = gpio_read_input(&PE8);
				armUpdate();
			}
			robotUpdate();
			if(autoPIDMode) {
				manualMode = false;
				updateQueue();
			}
			if(laserAuto){
				manualMode = false;
				enterPole();
			}
			if(autoModeLaser) {
				laserPID();
			}
			
			if (return_listener()) {
					return; 
				}
		}
		
	}
}

/**
  * @brief TFT LCD screen update function, can be in debug mode or not
  */

void _updateScreen() {
	tft_clear();
	
	#ifdef DEBUG_MODE
	//tft_prints(0, 6, "debug %d", xbc_get_connection());
	if (can_xbc_get_joy(XBC_JOY_LX) != 0) {
		tft_prints(0, 1, "LX %d", xbc_get_joy(XBC_JOY_LX));
	}
	if (can_xbc_get_joy(XBC_JOY_LY) != 0) {
		tft_prints(0, 2, "LY %d", xbc_get_joy(XBC_JOY_LY));
	}
	if (can_xbc_get_joy(XBC_JOY_RX) != 0) {
		tft_prints(9, 1, "RX %d", xbc_get_joy(XBC_JOY_RX));
	}
	if (can_xbc_get_joy(XBC_JOY_RY) != 0) {
		tft_prints(9, 2, "RY %d", xbc_get_joy(XBC_JOY_RY));
	}
	if (button_pressed(BUTTON_XBC_LB)) {
		tft_prints(0, 3, "LB");
	}
	if (button_pressed(BUTTON_XBC_RB)) {
		tft_prints(9, 3, "RB");
	}
	if (button_pressed(BUTTON_XBC_Y)) {
		tft_prints(0, 4, "Y");
	}
	if (button_pressed(BUTTON_XBC_X)) {
		tft_prints(2, 4, "X");
	}
	if (button_pressed(BUTTON_XBC_B)) {
		tft_prints(4, 4, "B");
	}
	if (button_pressed(BUTTON_XBC_A)) {
		tft_prints(6, 4, "A");
	}
	if (can_xbc_get_joy(XBC_JOY_LT) != 0) {
		tft_prints(0, 5, "LT %d" , getBrushlessMagnitude());
	}
	if (can_xbc_get_joy(XBC_JOY_RT) != 0) {
		tft_prints(6, 5, "RT %d", getBrushlessMagnitude());
	}
	tft_prints(0, 6, "%d %d %d", getWheelbaseValues().M1.sent, getWheelbaseValues().M2.sent, getWheelbasealues().M3.sent);
	tft_prints(16, 6, "%d", get_ticks());
	#else
	tft_prints(0, 0, "FIERY DRAGON");
	tft_prints(0, 1, "M: %d|%d|%d" , getWheelbaseValues().M1.sent, getWheelbaseValues().M2.sent, getWheelbaseValues().M3.sent);
	tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1, getPneumaticState().P2, getPneumaticState().P3, getPneumaticState().P4);
	tft_prints(0, 4, "G: %d|%d|%d", get_pos()->x, get_pos()->y, get_pos()->angle);
	tft_prints(0, 5, "LS: %d|%d|%d|%d|%d", gpio_read_input(&PE6), gpio_read_input(&PE7),  prevLimitSwitch[2], prevLimitSwitch[3], armIr); 
	tft_prints(0, 6, (robotMode == RED_SIDE) ? "MODE: RED SIDE" : "MODE:BLUE SIDE");
	tft_prints(0, 7, "L: %d|%d |LM: %d", get_ls_cal_reading(0), get_ls_cal_reading(1), autoModeLaser);
	//tft_prints(0, 7, "L: %d | AL: %d", get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1), laserAuto);
	//tft_prints(0, 8, "Q|ENC: %d|%d", getSize(), get_encoder_value(MOTOR8));
	tft_prints(0,8, "CAL: %d|%d|%d", get_ls_cal_reading(0), get_ls_cal_reading(1), get_ls_cal_reading(2));
	tft_prints(0, 9, "Blow Time : %d", blowTime);
	//tft_prints(0, 9, "adc: %d|%d|%d", get_ls_adc_reading(0), get_ls_adc_reading(1), get_ls_adc_reading(2));
	//tft_prints(0, 9, "B: %d| W: %d", laserB, laserW);
	//tft_prints(0, 9, "T: %d|%d|%d" , getWheelbaseValues().M1.target, getWheelbaseValues().M2.target, getWheelbaseValues().M3.target);
	//tft_prints(0, 9, "%d", getRotationValue());
	//tft_prints(0, 9, "RTZ: %d|%d", dispM, dispW);
	#endif
	tft_update();
}

/**
  * @brief Input listener for specific buttons
  */

void controllerInputUpdate() {
	
	//Button LB, RB
	if(button_pressed(BUTTON_XBC_RB)) {
		sendArmCommand(40);
	}
	else if(button_released(BUTTON_XBC_RB)) {
		sendArmCommand(0);
	}
	if(button_pressed(BUTTON_XBC_LB)) {
		sendArmCommand(-40);
	}
	else if(button_released(BUTTON_XBC_LB)) {
		sendArmCommand(0);
	}
	
	//Button LT, RT
	if(can_xbc_get_joy(XBC_JOY_LT) == 255 && _allowUpdate) {
		_allowUpdate = false;
		setBrushlessMagnitude((getBrushlessMagnitude() <=96)  ? getBrushlessMagnitude() + 4 : 100);
	}
	if(can_xbc_get_joy(XBC_JOY_RT) == 255 && _allowUpdate) {
		_allowUpdate = false;
		setBrushlessMagnitude((getBrushlessMagnitude() >=4)  ? getBrushlessMagnitude() - 4 : 0);
	}
	if(can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0 && !_allowUpdate) {
		_allowUpdate = true;
	}
	
	//D-Pad Buttons
	if(button_pressed(BUTTON_XBC_N) && allowDPadUpdate){
		allowDPadUpdate = false;
		pneumatics.P1 = !pneumatics.P1;
	}
	else if(button_released(BUTTON_XBC_N) && !allowDPadUpdate){
		allowDPadUpdate = true;
	}
	if(button_pressed(BUTTON_XBC_E) && allowDPadUpdate){
		allowDPadUpdate = false;
		pneumatics.P2 = !pneumatics.P2;
	}
	else if(button_released(BUTTON_XBC_E) && !allowDPadUpdate){
		allowDPadUpdate = true;
	}
	if(button_pressed(BUTTON_XBC_W) && allowDPadUpdate){
		allowDPadUpdate = false;
		pneumatics.P3 = !pneumatics.P3;
	}
	else if(button_released(BUTTON_XBC_W) && !allowDPadUpdate){
		allowDPadUpdate = true;
	}
	if(button_pressed(BUTTON_XBC_S) && allowDPadUpdate){
		allowDPadUpdate = false;
		pneumatics.P4 = !pneumatics.P4;
	}
	else if(button_released(BUTTON_XBC_S) && !allowDPadUpdate){
		allowDPadUpdate = true;
	}
	
	//Button A, B, X, Y
			if(button_pressed(BUTTON_XBC_Y) && !allowPIDUpdate){
				allowPIDUpdate = true;
				
				
//				autoModeLaser = !autoModeLaser;
//				manualMode = autoModeLaser ? false : true;
//				wheelbaseLock();
//				allowArm = true;
				if(getSize() == 0) {
					autoPIDMode = true;
					allowArm = true;
					manualMode = false;
					if(robotMode == RED_SIDE) {
						setBrushlessMagnitude(10);
						queueTargetPoint(650, 400, 90, -1, -1, -1, 0);
						queueTargetPoint(3121, 200, 90, 75, 50, 15, 0);
						queueTargetPoint(2865, 2152, 75, -1, -1, 15, 0);
						queueTargetPoint(2169, 3266, 44, -1, -1, 10, 0);
						queueTargetPoint(1553, 3960, 57, -1, -1, 16, 0);
						queueTargetPoint(989, 5117, 74, -1, -1, 7, 0);
						queueTargetPoint(242, 5662, 160, 610, 200, -1, 0);
						queueTargetPoint(625, 7505, 175 , 35.0, 5.0, -1, 6000);
						queueTargetPoint(242, 11000, 160, 500, 200 , -1, 0);//lost point
						queueTargetPoint(575, 8508, 160, 500, 200 , -1, 0);
						queueTargetPoint(1653, 12046, 272, 800, 200, -1, 0);
						queueTargetPoint(4359, 12610, 274, 800, 200, -1, 0);
						queueTargetPoint(5019, 12660, 274, 240, 120.0, -1, 0);
					}
					else if(robotMode == BLUE_SIDE) {
						setBrushlessMagnitude(14);
						queueTargetPoint(-650, 400, 270, -1, -1, -1, 0);
						queueTargetPoint(-3091, 300, 270, 35, 15, 14, 0);
						queueTargetPoint(-3091, 400, 270, 50, 50, 14, 0);
						queueTargetPoint(-2715, 1872, 285, -1, -1, 11, 0); //12 //75, 20
						//queueTargetPoint(-2175, 1872, 285, 450, 200, 15, 0);
//						queueTargetPoint(-2189, 3116, 316, -1, -1, 10, 0);
//						queueTargetPoint(-1513, 3830, 303, -1, -1, 12, 0);
//						queueTargetPoint(-659, 5017, 286, -1, -1, 7, 0);
//						queueTargetPoint(150, 5562, 200, 610, 200, -1, 0);
//						queueTargetPoint(55, 7505, 185, 35.0, 5.0, -1, 6000);
//						queueTargetPoint(-142, 11000, 200, 500, 200, -1, 0);//lost point
//						queueTargetPoint(50, 8508, 200, 500, 200, -1, 0);
//						queueTargetPoint(-503, 12046, 88, 800, 200, -1, 0);
//						queueTargetPoint(-1200, 12810, 86, 800, 200, -1, 0);
//						queueTargetPoint(-3010, 12810, 86, 240, 120.0, -1, 0);
					}
				}
				else {
					for(int i = getSize(); i>=1; i--) {
						dequeue(getSize());
					}
					manualMode = true;
				}
			}
			else if(button_released(BUTTON_XBC_Y) && allowPIDUpdate) {
				allowPIDUpdate = false;
			}
			if(button_pressed(BUTTON_XBC_B))
		{
			sendClimbCommand(1200);
		}
		else if(button_released(BUTTON_XBC_B))
		{
			sendClimbCommand(0);
		}
		if(button_pressed(BUTTON_XBC_X) && !allowArmUpdate) {
			allowArmUpdate = true;
			allowArm = !allowArm;
		}
		else if(button_released(BUTTON_XBC_X) && allowArmUpdate) {
			allowArmUpdate = false;
		}
		if(button_pressed(BUTTON_XBC_A) && !allowAutoLazers) {
			laserAuto = !laserAuto;
			manualMode = (laserAuto ? false : true);
			allowAutoLazers = true;
		}
		else if (button_released(BUTTON_XBC_A) && allowAutoLazers) {
			allowAutoLazers = false;
		}
		
	//XBOX Button
		if(button_pressed(BUTTON_XBC_XBOX) && !allowModeUpdate) {
			allowModeUpdate = true;
			robotMode = !robotMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && allowModeUpdate) {
			allowModeUpdate = false;
		}
		
//		if(can_xbc_get_joy(XBC_JOY_RX) == 0 && allowAngleUpdate) {
//			setTargetAngle(get_pos()->angle);
//			rightJoyInUse = false;
//			allowAngleUpdate = false;
//		}
//		else if(can_xbc_get_joy(XBC_JOY_RX) != 0 && !allowAngleUpdate) {
//			rightJoyInUse = true;
//			allowAngleUpdate = true;
//		}
		
}
