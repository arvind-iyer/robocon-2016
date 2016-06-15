#include "robocon.h"
#include "pk/pk.h"

extern int dispCurrentDistance;
extern int dispCurrentBearing;
extern int dispW;
extern int timediff;
extern bool laserAuto;

bool m_listener = false, m_holdListener = false, m_brushlessListener = false;

bool robotMode = RED_SIDE;
bool rightJoyInUse = false;
bool semiAuto = false;
bool benMode = false;

//Variables for certain mode triggers
bool allowArm = false;
bool benUpdate = false;
bool climbing = false;

int timeSinceButtonPressed = 0;

STATEMODES currMode = MANUAL;

STAGES currStage = STAGE1;

void robocon_main(void) {
	tft_init(0, BLACK, WHITE, SKY_BLUE);
	pk_init();
	pneumatics.P3 = true;
	pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
	while (1) {
		if (can_xbc_get_connection() != CAN_XBC_ALL_CONNECTED) {
			wheelbaseLock();
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
//		if (manualMode) {
//			lockBearing(LOCK);
//			manualControl();
//		} else {
//			lockBearing(UNLOCK);
//		}
		if (allowArm) {
			armIr = gpio_read_input(&PE8);
			armUpdate();
		}
		robotUpdate();

		if (get_full_ticks() % 10 == 0) {
			_updateScreen();
			hybridPneumaticControl();
		}
		if (get_full_ticks() % 3 == 0) {
			limitSwitchCheck();
			button_update();
			controllerInputUpdate();
			sendWheelbaseCommand();
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
	tft_prints(0, 0, "FIERY DRAGON |%s", benMode ? "BEN" : "NO");
	tft_prints(0, 1, "M: %d|%d|%d", getWheelbaseValues().M1.sent,
			getWheelbaseValues().M2.sent, getWheelbaseValues().M3.sent);
	tft_prints(0, 2, "B: %d | ARM: %d", getBrushlessMagnitude(), allowArm);
	//tft_prints(0, 3, "R : %d|%d|%d", get_pos_raw()->x, get_pos_raw()->y, get_pos_raw()->angle);
	tft_prints(0, 3, "P: %d|%d|%d|%d", getPneumaticState().P1,
			getPneumaticState().P2, getPneumaticState().P3,
			getPneumaticState().P4);
	tft_prints(0, 4, "G: %d|%d|%d", get_pos()->x, get_pos()->y,
			get_pos()->angle);
	tft_prints(0, 5, "LS: %d|%d|%d|%d|%d", prevLimitSwitch[0],
			prevLimitSwitch[1], prevLimitSwitch[2], prevLimitSwitch[3], armIr);

	tft_prints(0, 6,
			(robotMode == RED_SIDE) ? "MODE: RED SIDE" : "MODE:BLUE SIDE");
	//tft_prints(0, 7, "L: %d|%d| Q:%d", autoModeLaser, fieldDetected, getSize());

//	tft_prints(0,6, "ADC: %d|%d", get_ls_adc_reading(0), get_ls_adc_reading(1));
//	tft_prints(0,7, "ADC: %d|%d", get_ls_adc_reading(2), get_ls_adc_reading(3));

//tft_prints(0, 7, "L: %d | AL: %d", get_ls_cal_reading((robotMode == RED_SIDE) ? 0 : 1), laserAuto);
	tft_prints(0, 7, "Q|ENC: %d|%d", getSize(), get_encoder_value(MOTOR8));
	tft_prints(0, 8, "CAL: %d|%d", get_ls_cal_reading(0),
			get_ls_cal_reading(1));
	tft_prints(0, 9, "CAL: %d|%d", get_ls_cal_reading(2),
			get_ls_cal_reading(3));
	//tft_prints(0,9, "BEN? : %d|%d", benMode, button_pressed(BUTTON_XBC_XBOX));
	//tft_prints(0, 9, "Blow Time : %d", blowTime);
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
	if (!benMode) {
		//Button LB, RB
		if (button_pressed (BUTTON_XBC_RB)) {
			sendArmCommand(40);
		} else if (button_released(BUTTON_XBC_RB)) {
			sendArmCommand(0);
		}
		if (button_pressed (BUTTON_XBC_LB)) {
			sendArmCommand(-40);
		} else if (button_released(BUTTON_XBC_LB)) {
			sendArmCommand(0);
		}

		//Button LT, RT
		if (can_xbc_get_joy(XBC_JOY_LT) == 255 && !m_brushlessListener) {
			m_brushlessListener = true;
			setBrushlessMagnitude(
					(getBrushlessMagnitude() <= 96) ?
							getBrushlessMagnitude() + 4 : 100);
		}
		if (can_xbc_get_joy(XBC_JOY_RT) == 255 && !m_brushlessListener) {
			m_brushlessListener = true;
			setBrushlessMagnitude(
					(getBrushlessMagnitude() >= 4) ?
							getBrushlessMagnitude() - 4 : 0);
		}
		if (can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0
				&& m_brushlessListener) {
			m_brushlessListener = false;
		}
	} else {
		//Button LB, RB
		if (can_xbc_get_joy(XBC_JOY_RT) == 255) {
			sendArmCommand(40);
		} else if (button_released (BUTTON_XBC_RB)) {
			sendArmCommand(0);
		}
		if (button_pressed(BUTTON_XBC_LB) && !m_listener) {
			m_listener = true;
			setBrushlessMagnitude(
					(getBrushlessMagnitude() <= 96) ?
							getBrushlessMagnitude() + 4 : 100);
		} else if (button_released(BUTTON_XBC_LB) && m_listener) {
			m_listener = false;
		}

		//Button LT, RT
		if (can_xbc_get_joy(XBC_JOY_LT) == 255 && !m_brushlessListener) {
			m_brushlessListener = true;
			setBrushlessMagnitude(
					(getBrushlessMagnitude() >= 4) ?
							getBrushlessMagnitude() - 4 : 0);
		}
		if (button_pressed(BUTTON_XBC_RB) && m_listener) {
			sendArmCommand(-40);
			m_listener = false;
		}
		if (can_xbc_get_joy(XBC_JOY_LT) == 0 && can_xbc_get_joy(XBC_JOY_RT) == 0
				&& m_brushlessListener) {
			sendArmCommand(0);
			m_brushlessListener = false;
		}
	}

	//D-Pad Buttons
	if (button_pressed(BUTTON_XBC_N) && !m_listener) {
		m_listener = true;
		pneumatics.P1 = !pneumatics.P1;
	} else if (button_released(BUTTON_XBC_N) && m_listener) {
		m_listener = false;
	}
	if (button_pressed(BUTTON_XBC_E) && !m_listener) {
		m_listener = true;
		pneumatics.P2 = !pneumatics.P2;
	} else if (button_released(BUTTON_XBC_E) && m_listener) {
		m_listener = false;
	}
	if (button_pressed(BUTTON_XBC_W) && !m_listener) {
		m_listener = true;
		pneumatics.P3 = !pneumatics.P3;
	} else if (button_released(BUTTON_XBC_W) && m_listener) {
		m_listener = false;
	}
	if (button_pressed(BUTTON_XBC_S) && !m_listener) {
		m_listener = true;
		pneumatics.P4 = !pneumatics.P4;
	} else if (button_released(BUTTON_XBC_S) && m_listener) {
		m_listener = false;
	}

	//Button A, B, X, Y
	if (button_pressed(BUTTON_XBC_Y) && !m_listener) {
		m_listener = true;
		if (currMode == MANUAL) {
			timeSinceButtonPressed = get_full_ticks();
			setBrushlessMagnitude(robotMode == RED_SIDE ? 7 : 7);
			currMode = FIRSTPOS;
		} else {
			currMode = MANUAL;
		}
	} else if (button_released(BUTTON_XBC_Y) && m_listener) {
		m_listener = false;
	}
	if (button_pressed(BUTTON_XBC_B) && !m_listener) {
		m_listener = true;
		sendClimbCommand(1200);
	} else if (button_released(BUTTON_XBC_B) && m_listener) {
		m_listener = false;
		sendClimbCommand(0);
	}
	if (button_pressed(BUTTON_XBC_X) && !m_listener) {
		m_listener = true;
		allowArm = !allowArm;
	} else if (button_released(BUTTON_XBC_X) && m_listener) {
		m_listener = false;
	}
	if (button_pressed(BUTTON_XBC_A) && !m_listener) {
		setBrushlessMagnitude(0);
		if (currMode == MANUAL) {
			currMode = POLELASER;
		} else if (currMode == POLELASER) {
			currMode = MANUAL;
		}
		m_listener = true;
		climbing = false;
		if (pneumatics.P1 != true) {
			pneumatics.P1 = true;
			pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
		}
		if (pneumatics.P3 != false) {
			pneumatics.P3 = false;
			pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
		}
	} else if (button_released(BUTTON_XBC_A) && m_listener) {
		m_listener = false;
	}

	//XBOX Button
	if (button_pressed(BUTTON_XBC_XBOX) && !m_listener) {
		m_listener = true;
		robotMode = !robotMode;
	} else if (button_released(BUTTON_XBC_XBOX) && m_listener) {
		m_listener = false;
	}

	if (button_pressed(BUTTON_XBC_XBOX) > 1000 && !m_holdListener) {
		m_holdListener = true;
		benMode = !benMode;
	} else if (button_released(BUTTON_XBC_XBOX) && m_holdListener)
		m_holdListener = false;

	//EMERGENCY BUTTON
	if (button_pressed(BUTTON_XBC_START) && !m_listener) {
		m_listener = true;
		currMode = MANUAL;
		dequeueAll();
		wheelbaseLock();
		setBrushlessMagnitude(0);
		sendArmCommand(0);
	} else if (button_released(BUTTON_XBC_START) && m_listener) {
		m_listener = false;
	}

	//Laser PID Trigger
	if (button_pressed(BUTTON_XBC_BACK) && !m_holdListener) {
		currMode = LASERPID;
		m_holdListener = true;
		semiAuto = true;
	} else if (button_released(BUTTON_XBC_BACK) && m_holdListener) {
		m_holdListener = false;
		semiAuto = false;
		currMode = MANUAL;
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
