/***
** This file is for the global update of xbox controller
** To make sure the emergency stop and mode changin is updated on time
** Please call xbc_global_update() after button_update(), at a regualar interval
**
** Control scheme:
** back button - emergency stop/resume
** XBOX button - switch control mode
***/

#include "xbc_control.h"

static LOCK_STATE emergency_lock = UNLOCKED;
static LOCK_STATE press_button_back = UNLOCKED;
static LOCK_STATE press_button_XBOX = UNLOCKED;
static CONTROL_STATE current_control_state = MANUAL_MODE;
static CAN_XBC_CONNECTION_MODE last_connection_state = XBC_DISCONNECTED;

void xbc_global_update(){
		
	//Check whether emergency lock is pressed
	if (button_pressed(BUTTON_XBC_BACK)){
		if (press_button_back == UNLOCKED){
			press_button_back = LOCKED;
			if (emergency_lock == UNLOCKED){
				emergency_lock = LOCKED;
				emergency_stop();
				buzzer_beep(750);
			}else{
				emergency_lock = UNLOCKED;
				buzzer_beep(1000);
			}
		}
	}else{
			press_button_back = UNLOCKED;
	}
	
	//Turn on the emergency lock if the xbox controller is loose
	CAN_XBC_CONNECTION_MODE this_connection_state = can_xbc_get_connection();
	if (this_connection_state != last_connection_state){
		if (this_connection_state == CAN_XBC_DISCONNECTED){
			emergency_lock = LOCKED;
			emergency_stop();
			buzzer_beep(2500);
		}else if(this_connection_state == CAN_XBC_ALL_CONNECTED){
			emergency_lock = UNLOCKED;
			manual_vel_set_zero();
			buzzer_beep(500);
		}
		last_connection_state = this_connection_state;
	}
	
	//Change mode auto <-> manual
	if (button_pressed(BUTTON_XBC_XBOX)){
		if (press_button_XBOX == UNLOCKED){
			press_button_XBOX = LOCKED;
			if (current_control_state == MANUAL_MODE){
				current_control_state = AUTO_MODE;
				//buzzer_beep(450);
			}else{
				current_control_state = MANUAL_MODE;
				//buzzer_beep(650);
			}
		}
	}else{
			press_button_XBOX = UNLOCKED;
	}
}

CONTROL_STATE get_control_state(){
	return current_control_state;
}

LOCK_STATE get_emergency_lock(){
	return emergency_lock;
}
