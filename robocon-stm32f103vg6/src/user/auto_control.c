#include "auto_control.h"

bool au_listening = false, au_holdListening = false, au_brushlessListening = false;

void auto_control() {
	tft_init(0, BLACK, WHITE, SKY_BLUE);
	pk_init();
	pneumatics.P3 = true;
	pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
	allow4thUpdate = true;
	//allowArm = true;
	while (1) {
		if (can_xbc_get_connection() != CAN_XBC_ALL_CONNECTED) {
			wheelbaseLock();
		}
		if (return_listener()) {
			return;
		}
		dataSampling();
		reset();
		switch (currMode) {
		case MANUAL:
			manualControl();
			break;
		case FIRSTPOS:
			moveToFirstPosition();
			break;
		case LASERPID:
			laserPID();
			break;
		case POLELASER:
			enterPole();
			break;
		case APPROACHWALL:
			moveToWall();
			break;
		case PIDMODE:
			updateQueue();
			break;
	}

		if (allowArm) {
			armIr = gpio_read_input(&PE8);
			armUpdate();
		}
		robotUpdate();

		if (get_full_ticks() % 10 == 0) {
			autoControlScreenUpdater();
			hybridPneumaticControl();
		}
		if (get_full_ticks() % 3 == 0) {
			limitSwitchCheck();
			button_update();
			autoControlListener();
			sendWheelbaseCommand();
		}

	}

}

void autoControlScreenUpdater() {
	tft_clear();
tft_prints(0, 0, "AUTO |%s", benMode ? "BEN" : "NO");
	//tft_prints(0, 1, "M: %d|%d|%d", getWheelbaseValues().M1.sent,
		//	getWheelbaseValues().M2.sent, getWheelbaseValues().M3.sent);
	//tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	//tft_prints(0, 3, "R : %d|%d|%d", get_pos_raw()->x, get_pos_raw()->y, get_pos_raw()->angle);
	//tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1,
		//	getPneumaticState().P2, getPneumaticState().P3,
			//getPneumaticState().P4);
	tft_prints(0, 1, "G: %d|%d|%d", get_pos()->x, get_pos()->y,
			get_pos()->angle);
	tft_prints(0, 2,
			(robotMode == RED_SIDE) ? "MODE: RED SIDE" : "MODE:BLUE SIDE");
	tft_prints(0, 3, "LS: %d|%d|%d|%d|%d", prevLimitSwitch[0],
			prevLimitSwitch[1], prevLimitSwitch[2], prevLimitSwitch[3], armIr);
	tft_prints(0, 4, "4th Read? : %d", allow4thUpdate);
	tft_prints(0, 7, "Q|ENC: %d|%d", getSize(), get_encoder_value(MOTOR8));
		tft_prints(0, 8, "CAL: %d|%d", get_ls_cal_reading(0),
			get_ls_cal_reading(1));
	tft_prints(0, 9, "CAL: %d|%d", get_ls_cal_reading(2),
			get_ls_cal_reading(3));
	//tft_prints(0, 8, "CAL: %d|%d", get_ls_cal_reading(0),
		//	get_ls_cal_reading(1));
	//tft_prints(0, 9, "CAL: %d|%d", get_ls_cal_reading(2),
		//	get_ls_cal_reading(3));
	tft_update();
}

void autoControlListener() {
//	if (!benMode) {
//		//Button LB, RB
//		if (button_pressed (BUTTON_XBC_RB)) {
//			sendArmCommand(40);
//		} else if (button_released(BUTTON_XBC_RB)) {
//			sendArmCommand(0);
//		}
//		if (button_pressed (BUTTON_XBC_LB)) {
//			sendArmCommand(-40);
//		} else if (button_released(BUTTON_XBC_LB)) {
//			sendArmCommand(0);
//		}

//		//Button LT, RT
//		if (can_xbc_get_joy(XBC_JOY_LT) == 255 && !au_brushlessListening) {
//			au_brushlessListening = true;
//			setBrushlessMagnitude(
//					(getBrushlessMagnitude() <= 96) ?
//							getBrushlessMagnitude() + 4 : 100);
//		}
//		if (can_xbc_get_joy(XBC_JOY_RT) == 255 && !au_brushlessListening) {
//			au = true;
//			setBrushlessMagnitude(
//					(getBrushlessMagnitude() >= 4) ?
//							getBrushlessMagnitude() - 4 : 0);
//		}
//		if (can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0
//				&& s_brushless_listening) {
//			s_brushless_listening = false;
//		}
//	} else {
//		//Button LB, RB
//		if (can_xbc_get_joy(XBC_JOY_RT) == 255) {
//			sendArmCommand(40);
//		} else if (button_released (BUTTON_XBC_RB)) {
//			sendArmCommand(0);
//		}
//		if (button_pressed(BUTTON_XBC_LB) && !s_listening) {
//			s_listening = true;
//			setBrushlessMagnitude(
//					(getBrushlessMagnitude() <= 96) ?
//							getBrushlessMagnitude() + 4 : 100);
//		} else if (button_released(BUTTON_XBC_LB) && s_listening) {
//			s_listening = false;
//		}

//		//Button LT, RT
//		if (can_xbc_get_joy(XBC_JOY_LT) == 255 && !s_brushless_listening) {
//			s_brushless_listening = true;
//			setBrushlessMagnitude(
//					(getBrushlessMagnitude() >= 4) ?
//							getBrushlessMagnitude() - 4 : 0);
//		}
//		if (button_pressed (BUTTON_XBC_RB)) {
//			sendArmCommand(-40);
//		}
//		if (can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0
//				&& s_brushless_listening) {
//			sendArmCommand(0);
//			s_brushless_listening = false;
//		}
//	}

	//D-Pad Buttons
	if (button_pressed(BUTTON_XBC_N) && !au_listening) {
		au_listening = true;
		pneumatics.P1 = !pneumatics.P1;
	} else if (button_released(BUTTON_XBC_N) && au_listening) {
		au_listening = false;
	}
	if (button_pressed(BUTTON_XBC_E) && !au_listening) {
		au_listening = true;
		pneumatics.P2 = !pneumatics.P2;
	} else if (button_released(BUTTON_XBC_E) && au_listening) {
		au_listening = false;
	}
	if (button_pressed(BUTTON_XBC_W) && !au_listening) {
		au_listening = true;
		pneumatics.P3 = !pneumatics.P3;
	} else if (button_released(BUTTON_XBC_W) && au_listening) {
		au_listening = false;
	}
	if (button_pressed(BUTTON_XBC_S) && !au_listening) {
		au_listening = true;
		allow4thUpdate = !allow4thUpdate;
	} else if (button_released(BUTTON_XBC_S) && au_listening) {
		au_listening = false;
	}

	//Button A, B, X, Y
//	if (button_pressed(BUTTON_XBC_B) && !au_listening) {
//		au_listening = true;
//		currMode = LASERPID;
//	} else if (button_released(BUTTON_XBC_B) && au_listening) {
//		au_listening = false;
//		currMode = MANUAL;
//	}
	if (button_pressed(BUTTON_XBC_Y) && !au_listening) {
		au_listening = true;
		if (currMode == MANUAL) {
			timeSinceButtonPressed = get_full_ticks();
			setBrushlessMagnitude(robotMode == RED_SIDE ? 7 : 7);
			currMode = FIRSTPOS;
		} else {
			currMode = MANUAL;
		}
	} else if (button_released(BUTTON_XBC_Y) && au_listening) {
		au_listening = false;
	}
//if (button_pressed(BUTTON_XBC_X) && !au_listening) {
//	au_listening = true;
//	allowArm = !allowArm;
//} else if (button_released(BUTTON_XBC_X) && au_listening) {
//	au_listening = false;
//}
	if (button_pressed(BUTTON_XBC_A) && !au_listening ) {
		setBrushlessMagnitude(0);
		if (currMode == MANUAL) {
			currMode = POLELASER;
		} else if (currMode == POLELASER) {
			currMode = MANUAL;
		}
		au_listening = true;
		climbing = false;
		slowdownDelay = get_full_ticks();
		if (pneumatics.P1 != true) {
			pneumatics.P1 = true;
			pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
		}
//		if (pneumatics.P3 != false && !allow4thUpdate) {
//			pneumatics.P3 = false;
//			pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
//		}
	} else if (button_released(BUTTON_XBC_A) && au_listening) {
		au_listening = false;
	}

	//XBOX Button
	if (button_pressed(BUTTON_XBC_XBOX) && !au_listening) {
		au_listening = true;
		robotMode = !robotMode;
	} else if (button_released(BUTTON_XBC_XBOX) && au_listening) {
		au_listening = false;
	}

	if (button_pressed(BUTTON_XBC_XBOX) > 1000 && !au_holdListening) {
		au_holdListening = true;
		benMode = !benMode;
	} else if (button_released(BUTTON_XBC_XBOX) && au_holdListening)
		au_holdListening = false;

	//EMERGENCY BUTTON
	if (button_pressed(BUTTON_XBC_START) && !au_listening) {
		au_listening = true;
		dequeueAll();
		wheelbaseLock();
		setBrushlessMagnitude(0);
		sendArmCommand(0);
		currMode = MANUAL;
	} else if (button_released(BUTTON_XBC_START) && au_listening) {
		au_listening = false;
	}
}
