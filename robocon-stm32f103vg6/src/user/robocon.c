#include "robocon.h"
#include "pk/pk.h"

extern int dispCurrentDistance;
extern int dispCurrentBearing;

//#define DEBUG_MODE
#define RED_SIDE 0
#define BLUE_SIDE 1

bool robotMode = false;

//Variables for allowing the update of the button
bool _allowUpdate = false, allowDPadUpdate = false, allowArm = false, allowArmUpdate = false, allowModeUpdate = false, allowPIDUpdate = false;

void robocon_main(void)
{	
		tft_init(0, BLACK, WHITE, SKY_BLUE);
		pk_init();
	
	while (1) {
		if(get_full_ticks()%25 == 0){
			button_update();
			_updateScreen();
			controllerInputUpdate();
			manualControl();
			hybridPneumaticControl();
			limitSwitchCheck();
			if(allowArm) {
				armIr = gpio_read_input(&PE8);
				armUpdate();
			}
			robotUpdate();
			updateQueue();
			
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
	tft_prints(0, 6, "%d %d %d", getMotorValues().M1, getMotorValues().M2, getMotorValues().M3);
	tft_prints(16, 6, "%d", get_ticks());
	#else
	tft_prints(0, 0, "FIERY DRAGON");
	tft_prints(0, 1, "M: %d|%d|%d" , getMotorValues().M1, getMotorValues().M2, getMotorValues().M3);
	tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1, getPneumaticState().P2, getPneumaticState().P3, getPneumaticState().P4);
	tft_prints(0, 4, "G: %d|%d|%d", get_pos()->x, get_pos()->y, get_pos()->angle);
	tft_prints(0, 5, "LS: %d|%d|%d|%d|%d", gpio_read_input(&PE6), gpio_read_input(&PE7),  prevLimitSwitch[2], prevLimitSwitch[3], armIr); 
	tft_prints(0, 6, (robotMode == RED_SIDE) ? "MODE: RED SIDE" : "MODE:BLUE SIDE");
	tft_prints(0, 7, "L: %d", get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1));
	tft_prints(0, 8, "Q: %d|%d|%d", getSize(), dispCurrentDistance, dispCurrentBearing);
	tft_prints(0, 9, "Time: %d", get_seconds());
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
	
	//Button A, B, X, Y
			if(button_pressed(BUTTON_XBC_Y) && !allowPIDUpdate){
				allowPIDUpdate = true;
				if(getSize() == 0) {
					queueTargetPoint(1000, 1000, 0, 35.0 , 10);
				}
				else {
					for(int i = getSize(); i>=1; i--) {
						dequeue(getSize());
					}
				}
			}
			else if(button_released(BUTTON_XBC_Y) && allowPIDUpdate) {
				allowPIDUpdate = false;
			}
			if(button_pressed(BUTTON_XBC_B))
		{
			sendClimbCommands(900, -900, 900, -900);
		}
		else if(button_released(BUTTON_XBC_B))
		{
			sendClimbCommands(0,0,0,0);
		}
		if(button_pressed(BUTTON_XBC_X) && !allowArmUpdate) {
			allowArmUpdate = true;
			allowArm = !allowArm;
		}
		else if(button_released(BUTTON_XBC_X) && allowArmUpdate) {
			allowArmUpdate = false;
		}
		
	//XBOX Button
		if(button_pressed(BUTTON_XBC_XBOX) && !allowModeUpdate) {
			allowModeUpdate = true;
			robotMode = !robotMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && allowModeUpdate) {
			allowModeUpdate = false;
		}
		
}