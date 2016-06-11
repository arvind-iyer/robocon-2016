/***
** This file is for the global update of xbox controller
** To make sure the emergency stop and mode changin is updated on time
** Please call xbc_global_update() after button_update(), at a regualar interval
**
** Also implemented the button input now
***/

#include "xbc_control.h"

static LOCK_STATE emergency_lock = UNLOCKED;
static CONTROL_STATE current_control_state = MANUAL_MODE;
static CAN_XBC_CONNECTION_MODE last_connection_state = CAN_XBC_DISCONNECTED;

static LOCK_STATE all_button_lock[XBC_BUTTON_COUNTS+XBC_BUTTON_START_ID] = {UNLOCKED};
bool button_hitted[XBC_BUTTON_COUNTS+XBC_BUTTON_START_ID] = {false};

void xbc_global_update(){

	//Turn on the emergency lock if the xbox controller is loose
	CAN_XBC_CONNECTION_MODE this_connection_state = can_xbc_get_connection();
	if (this_connection_state != last_connection_state){
		if (this_connection_state == CAN_XBC_DISCONNECTED || this_connection_state == CAN_XBC_CAN_CONNECTED){
			emergency_lock = LOCKED;
			emergency_stop();
			buzzer_play_song(CRAZY_YELL, 100, 0);
		}else if(this_connection_state == CAN_XBC_ALL_CONNECTED){
			emergency_lock = UNLOCKED;
			manual_vel_set_zero();
			buzzer_play_song(START_UP2, 100, 0);
		}
		last_connection_state = this_connection_state;
	}
}

void xbc_input_update(){
	for (u8 i=0; i<(XBC_BUTTON_COUNTS+XBC_BUTTON_START_ID); i++){
		if (button_pressed((BUTTON)i)){
			if (all_button_lock[i] == UNLOCKED){
				all_button_lock[i] = LOCKED;
				button_hitted[i] = true;
			}else{
				button_hitted[i] = false;
			}
		}else{
			all_button_lock[i] = UNLOCKED;
			button_hitted[i] = false;
		}
	}
}

CONTROL_STATE get_control_state(){
	return current_control_state;
}

LOCK_STATE get_emergency_lock(){
	return emergency_lock;
}


