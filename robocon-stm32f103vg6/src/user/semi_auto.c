#include "semi_auto.h"

bool s_listening = false, s_brushless_listening = false, s_holds_listening = false;

void semi_auto_control () {
	tft_init(0, BLACK, WHITE, SKY_BLUE);
		pk_init();
		pneumatics.P3 = true;
		pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
		semiAuto = true;
	while (1) {
		if (can_xbc_get_connection() != CAN_XBC_ALL_CONNECTED) {
			wheelbaseLock();
		}
		if (return_listener()) {
					return; 
				}
		dataSampling();
		reset();
		if (manualMode) {
			lockBearing(LOCK);
			manualControl();
		} else {
			lockBearing(UNLOCK);
		}
		if(laserAuto){
			manualMode = false;
			enterPole();
		}
		if(autoModeLaser) {
			manualMode = false;
			laserPID();
		}
		
		if(allowArm) {
			armIr = gpio_read_input(&PE8);
			armUpdate();
		}
		robotUpdate();
		
		if(get_full_ticks()%10 == 0){
			semiAutoScreenUpdater();
			hybridPneumaticControl();
		}
		if(get_full_ticks()%3 == 0) {
			limitSwitchCheck();
			button_update();
			semiAutoListener();
			sendWheelbaseCommand();
		}
		
	}
	
}

	void semiAutoScreenUpdater () {
	tft_clear();
	tft_prints(0, 0, "SEMI AUTO|%s", benMode ? "BEN" : "NOBEN");
	tft_prints(0, 1, "M: %d|%d|%d" , getWheelbaseValues().M1.sent, getWheelbaseValues().M2.sent, getWheelbaseValues().M3.sent);
	tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1, getPneumaticState().P2, getPneumaticState().P3, getPneumaticState().P4);
	tft_prints(0, 4, (robotMode == RED_SIDE) ? "MODE:RED SIDE" : "MODE:BLUE SIDE");
	tft_prints(0, 5, "LS: %d|%d|%d|%d|%d", prevLimitSwitch[0], prevLimitSwitch[1],  prevLimitSwitch[2], prevLimitSwitch[3], armIr); 
	tft_prints(0, 6, "CAL: %d|%d", get_ls_cal_reading(0), get_ls_cal_reading(1));
	tft_prints(0, 7, "CAL: %d|%d", get_ls_cal_reading(2), get_ls_cal_reading(3));
	tft_update();
	}


	void semiAutoListener () {
		if(!benMode) {
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
				if(can_xbc_get_joy(XBC_JOY_LT) == 255 && !s_brushless_listening) {
					s_brushless_listening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() <=96)  ? getBrushlessMagnitude() + 4 : 100);
				}
				if(can_xbc_get_joy(XBC_JOY_RT) == 255 && !s_brushless_listening) {
					s_brushless_listening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() >=4)  ? getBrushlessMagnitude() - 4 : 0);
				}
				if(can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0 && s_brushless_listening) {
					s_brushless_listening = false;
				}
	}
	else {
					//Button LB, RB
				if(can_xbc_get_joy(XBC_JOY_RT) == 255) {
					sendArmCommand(40);
				}
				else if(button_released(BUTTON_XBC_RB)) {
					sendArmCommand(0);
				}
				if(button_pressed(BUTTON_XBC_LB) && !s_listening) {
					s_listening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() <=96)  ? getBrushlessMagnitude() + 4 : 100);
				}
				else if(button_released(BUTTON_XBC_LB) && s_listening) {
					s_listening = false;
				}
				
				//Button LT, RT
				if(can_xbc_get_joy(XBC_JOY_LT) == 255 && !s_brushless_listening) {
					s_brushless_listening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() >=4)  ? getBrushlessMagnitude() - 4 : 0);
				}
				if(button_pressed(BUTTON_XBC_RB)) {
					sendArmCommand(-40);
				}
				if(can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0 && s_brushless_listening) {
					sendArmCommand(0);
					s_brushless_listening = false;
				}
	}
	
	
	//D-Pad Buttons
	if(button_pressed(BUTTON_XBC_N) && !s_listening){
		s_listening = true;
		pneumatics.P1 = !pneumatics.P1;
	}
	else if(button_released(BUTTON_XBC_N) && s_listening){
		s_listening = false;
	}
	if(button_pressed(BUTTON_XBC_E) && !s_listening){
		s_listening = true;
		pneumatics.P2 = !pneumatics.P2;
	}
	else if(button_released(BUTTON_XBC_E) && s_listening){
		s_listening = false;
	}
	if(button_pressed(BUTTON_XBC_W) && !s_listening){
		s_listening = true;
		pneumatics.P3 = !pneumatics.P3;
	}
	else if(button_released(BUTTON_XBC_W) && s_listening){
		s_listening = false;
	}
	if(button_pressed(BUTTON_XBC_S) && !s_listening){
		s_listening = true;
		pneumatics.P4 = !pneumatics.P4;
	}
	else if(button_released(BUTTON_XBC_S) && s_listening){
		s_listening = false;
	}
	
	//Button A, B, X, Y
	if(button_pressed(BUTTON_XBC_B) && !s_listening) {
			s_listening = true;
			autoModeLaser = true;
			//semiAuto = true;
			manualMode = false;
		}
		else if(button_released(BUTTON_XBC_B) && s_listening) {
			s_listening = false;
			//semiAuto = false;
			autoModeLaser = false;
			manualMode = true;
		}
		if(button_pressed(BUTTON_XBC_Y) && !s_listening)
			{
				s_listening = true;
				sendClimbCommand(1200);
			}
		else if(button_released(BUTTON_XBC_Y) && s_listening)
		{
			s_listening = false;
			sendClimbCommand(0);
		}
		if(button_pressed(BUTTON_XBC_X) && !s_listening) {
			s_listening = true;
			allowArm = !allowArm;
		}
		else if(button_released(BUTTON_XBC_X) && s_listening) {
			s_listening = false;
		}
		if(button_pressed(BUTTON_XBC_A) && !s_listening) {
			setBrushlessMagnitude(0);
			laserAuto = !laserAuto;
			manualMode = (laserAuto ? false : true);
			s_listening = true;
			climbing = false;
			if(pneumatics.P1 != true) {
				pneumatics.P1 = true;
				pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
			}
			if(pneumatics.P3 != false) {
					pneumatics.P3 = false;
					pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
				}
		}
		else if (button_released(BUTTON_XBC_A) && s_listening) {
			s_listening = false;
		}
		
	//XBOX Button
		if(button_pressed(BUTTON_XBC_XBOX) && !s_listening) {
			s_listening = true;
			robotMode = !robotMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && s_listening) {
			s_listening = false;
		}
		
		if(button_pressed(BUTTON_XBC_XBOX) > 1000 && !s_holds_listening) {
			s_holds_listening = true;
			benMode = !benMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && s_holds_listening) s_holds_listening = false;
		
		//EMERGENCY BUTTON
		if(button_pressed(BUTTON_XBC_START) && !s_listening) {
			s_listening = true;
			wallApproach = false;
			dequeueAll();
			wheelbaseLock();
			setBrushlessMagnitude(0);
			sendArmCommand(0);
			manualMode = true;
			laserAuto = false;
			autoModeLaser = false;
			benMode = false;
			autoPIDMode = false;
			approachFirstPosition = false;
		}
		else if(button_released(BUTTON_XBC_START) && s_listening) {
			s_listening = false;
		}
	}
	