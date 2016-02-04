#include "xbc_control.h"

LOCK_STATE emergency_lock = UNLOCKED;
LOCK_STATE press_button_back = UNLOCKED;
LOCK_STATE press_button_XBOX = UNLOCKED;
CONTROL_STATE current_control_state = MANUAL_MODE;

void xbc_global_update(){
	//E-Stop
	tft_append_line("%d %d %d", button_pressed(BUTTON_XBC_BACK), emergency_lock, press_button_back);
	tft_append_line("2:%d", get_ticks());
	tft_update();
		
	if (button_pressed(BUTTON_XBC_BACK)){
		if (press_button_back == UNLOCKED){
			press_button_back = LOCKED;
			if (emergency_lock == UNLOCKED){
				emergency_lock = LOCKED;
				buzzer_beep(750);
			}else{
				emergency_lock = UNLOCKED;
				buzzer_beep(1000);
			}
		}
	}else{
			press_button_back = UNLOCKED;
	}
	
	//Change mode
	if (button_pressed(BUTTON_XBC_XBOX)){
		if (press_button_XBOX == UNLOCKED){
			press_button_XBOX = LOCKED;
			if (current_control_state == MANUAL_MODE){
				current_control_state = AUTO_MODE;
				buzzer_beep(450);
			}else{
				current_control_state = MANUAL_MODE;
				buzzer_beep(650);
			}
		}
	}else{
			press_button_XBOX = UNLOCKED;
	}
	tft_append_line("%d %d", emergency_lock, press_button_back);
}

CONTROL_STATE get_control_state(){
	return current_control_state;
}

LOCK_STATE get_emergency_lock(){
	return emergency_lock;
}