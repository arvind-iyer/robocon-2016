#include "manual.h"

bool listening = false, brushlessListening = false, holdListening = false;

void manual_control () {
		tft_init(0, BLACK, WHITE, SKY_BLUE);
		pk_init();
		pneumatics.P3 = true;
		pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
		allowArm = true;
	while (1) {
		if (can_xbc_get_connection() != CAN_XBC_ALL_CONNECTED) {
				wheelbaseLock();
			}
		if (return_listener()) {
					return; 
				}
		dataSampling();
		reset();
//		if (manualMode) {
//			lockBearing(LOCK);
//			manualControl();
//		} else {
//			lockBearing(UNLOCK);
//		}
		switch (currMode) {
			case MANUAL:
				manualControl();
				break;
			case POLELASER:
				enterPole();
				break;
			default:
				break;
		}
		
		if(allowArm) {
			armIr = gpio_read_input(&PE8);
			armUpdate();
		}
		robotUpdate();
		
		if(get_full_ticks()%10 == 0){
			manualControlScreenUpdater();
			hybridPneumaticControl();
		}
		if(get_full_ticks()%3 == 0) {
			limitSwitchCheck();
			button_update();
			manualControlListener();
			sendWheelbaseCommand();
		}
		
	}
	
}

	void manualControlScreenUpdater () {
	tft_clear();
	tft_prints(0, 0, "MANUAL|%s", benMode ? "BEN" : "NOBEN");
	tft_prints(0, 1, "M: %d|%d|%d" , getWheelbaseValues().M1.sent, getWheelbaseValues().M2.sent, getWheelbaseValues().M3.sent);
	tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1, getPneumaticState().P2, getPneumaticState().P3, getPneumaticState().P4);
	tft_prints(0, 4, (robotMode == RED_SIDE) ? "MODE:RED SIDE" : "MODE:BLUE SIDE");
	tft_prints(0, 5, "LS: %d|%d|%d|%d|%d", prevLimitSwitch[0], prevLimitSwitch[1],  prevLimitSwitch[2], prevLimitSwitch[3], armIr); 
	tft_prints(0, 6, "CAL: %d|%d", get_ls_cal_reading(0), get_ls_cal_reading(1));
	tft_prints(0, 7, "CAL: %d|%d", get_ls_cal_reading(2), get_ls_cal_reading(3));
		tft_prints(0, 8, "4th Read? : %d", allow4thUpdate);
	tft_update();
	}


	void manualControlListener () {
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
				if(can_xbc_get_joy(XBC_JOY_LT) == 255 && !brushlessListening) {
					brushlessListening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() <=96)  ? getBrushlessMagnitude() + 4 : 100);
				}
				if(can_xbc_get_joy(XBC_JOY_RT) == 255 && !brushlessListening) {
					brushlessListening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() >=4)  ? getBrushlessMagnitude() - 4 : 0);
				}
				if(can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0 && brushlessListening) {
					brushlessListening = false;
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
				if(button_pressed(BUTTON_XBC_LB) && !listening) {
					listening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() <=96)  ? getBrushlessMagnitude() + 4 : 100);
				}
				else if(button_released(BUTTON_XBC_LB) && listening) {
					listening = false;
				}
				
				//Button LT, RT
				if(can_xbc_get_joy(XBC_JOY_LT) == 255 && !brushlessListening) {
					brushlessListening = true;
					setBrushlessMagnitude((getBrushlessMagnitude() >=4)  ? getBrushlessMagnitude() - 4 : 0);
				}
				if(button_pressed(BUTTON_XBC_RB)) {
					sendArmCommand(-40);
				}
				if(can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0 && brushlessListening) {
					sendArmCommand(0);
					brushlessListening = false;
				}
	}
	
	
	//D-Pad Buttons
	if(button_pressed(BUTTON_XBC_N) && !listening){
		listening = true;
		pneumatics.P1 = !pneumatics.P1;
	}
	else if(button_released(BUTTON_XBC_N) && listening){
		listening = false;
	}
	if(button_pressed(BUTTON_XBC_E) && !listening){
		listening = true;
		pneumatics.P2 = !pneumatics.P2;
	}
	else if(button_released(BUTTON_XBC_E) && listening){
		listening = false;
	}
	if(button_pressed(BUTTON_XBC_W) && !listening){
		listening = true;
		pneumatics.P3 = !pneumatics.P3;
	}
	else if(button_released(BUTTON_XBC_W) && listening){
		listening = false;
	}
	if(button_pressed(BUTTON_XBC_S) && !listening){
		listening = true;
		allow4thUpdate = !allow4thUpdate;
	}
	else if(button_released(BUTTON_XBC_S) && listening){
		listening = false;
	}
	
	//Button A, B, X, Y
		if(button_pressed(BUTTON_XBC_B) && !listening)
			{
				listening = true;
				sendClimbCommand(1200);
			}
		else if(button_released(BUTTON_XBC_B) && listening)
		{
			listening = false;
			sendClimbCommand(0);
		}
		if(button_pressed(BUTTON_XBC_X) && !listening) {
			listening = true;
			allowArm = !allowArm;
		}
		else if(button_released(BUTTON_XBC_X) && listening) {
			listening = false;
		}
		if(button_pressed(BUTTON_XBC_A) && !listening) {
			setBrushlessMagnitude(0);
			if(currMode == MANUAL) {
				currMode = POLELASER;
			}
			else if (currMode == POLELASER) {
				currMode = MANUAL;
			}
			listening = true;
			climbing = false;
			slowdownDelay = get_full_ticks();
			if(pneumatics.P1 != true) {
				pneumatics.P1 = true;
				pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
			}
//			if(pneumatics.P3 != false && !allow4thUpdate) {
//					pneumatics.P3 = false;
//					pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
//				}
		}
		else if (button_released(BUTTON_XBC_A) && listening) {
			listening = false;
		}
		
	//XBOX Button
		if(button_pressed(BUTTON_XBC_XBOX) && !listening) {
			listening = true;
			robotMode = !robotMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && listening) {
			listening = false;
		}
		
		if(button_pressed(BUTTON_XBC_XBOX) > 1000 && !holdListening) {
			holdListening = true;
			benMode = !benMode;
		}
		else if(button_released(BUTTON_XBC_XBOX) && holdListening) holdListening = false;
		
		//EMERGENCY BUTTON
		if(button_pressed(BUTTON_XBC_START) && !listening) {
			listening = true;
			dequeueAll();
			wheelbaseLock();
			setBrushlessMagnitude(0);
			sendArmCommand(0);
			currMode = MANUAL;
		}
		else if(button_released(BUTTON_XBC_START) && listening) {
			listening = false;
		}
		
	}
	