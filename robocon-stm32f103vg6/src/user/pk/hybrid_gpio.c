#include "hybrid_gpio.h"
#include "pneumatic.h"
#include "gpio.h"
#include "ticks.h"
#include "can_motor.h"
#include "hybrid_laser.h"
#include "pk_wheelbase.h"
#include "pk_arm.h"
#include "pk_climb.h"
#include "robocon.h"
#include "ticks.h"
#include "control.h"

bool limitSwitch[4] = { false, false, false, false };
bool prevLimitSwitch[4] = { false, false, false, false };
bool armIr = false, prevArmIr = false;
bool retryIr = false, prevRetryIr = false;
int climbDelay = 0;
int moveDelay = 0;
int climbingState = PREPARATION;
bool allow4thUpdate = false;

/**
 * @brief Initializes the Hybrid's GPIO ports and some required variables
 */

void hybridGPIOInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	// Limit switch GPIO initialization.
	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);
	gpio_init(&PE9, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);
	gpio_init(&PE11, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);
	// IR Sensor GPIO initialization.
	gpio_init(&PE8, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);
	//Retry IR
	gpio_init(&PE0, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);
}

bool armDir = false, fixingArm = false, climbLimit = false; // False = down, True = up
long lastLimitCheck = 0;
int waitDelay = 0;

/**
 * @brief Checks all the limit switches on the hybrid
 */

void limitSwitchCheck() {
	limitSwitch[0] = !gpio_read_input(&PE6);
	limitSwitch[1] = !gpio_read_input(&PE7);
	limitSwitch[2] = !gpio_read_input(&PE9);
	if (allow4thUpdate)
		limitSwitch[3] = !gpio_read_input(&PE11);
	if (get_full_ticks() - lastLimitCheck >= 100 && (fixingArm)) {
		fixingArm = false;
		climbLimit = false;
		motor_set_vel(MOTOR8, 0, CLOSE_LOOP);
		lastLimitCheck = get_full_ticks();
	}
	prevLimitSwitch[0] = (
			prevLimitSwitch[0] != limitSwitch[0] ?
					limitSwitch[0] : prevLimitSwitch[0]);
	prevLimitSwitch[1] = (
			prevLimitSwitch[1] != limitSwitch[1] ?
					limitSwitch[1] : prevLimitSwitch[1]);
	if (limitSwitch[0] || limitSwitch[1]) {
		armDir = limitSwitch[1] ? 1 : 0;
		motor_set_vel(MOTOR8, armDir ? -10 : 10, CLOSE_LOOP);
		fixingArm = true;
		lastLimitCheck = get_full_ticks();
		// If fixing arm, always make sure IR never gets triggered until fixingArm is false.
	}
	if (prevLimitSwitch[2] != limitSwitch[2]) {
		prevLimitSwitch[2] = limitSwitch[2];
		sendClimbCommand(0);
		if (limitSwitch[2] == true) {
			//topHit = true;
			waitDelay = get_full_ticks();
		}

		//climbLimit = true;	
		//lastLimitCheck = get_full_ticks();
	}
	if (prevLimitSwitch[3] != limitSwitch[3]) {
		prevLimitSwitch[3] = limitSwitch[3];
		moveDelay = get_full_ticks();
		currMode = MANUAL;
		if (climbingState == PREPARATION)
			climbingState = GRIP_POLE;
	}
	if (climbingState == GRIP_POLE) {
		double angularVelocity = getAngleDifference(robot.position.angle,
				robotMode == RED_SIDE ? 270 : 90) * 50 / 180 * -1;
		if (Abs(angularVelocity) >= 50)
			angularVelocity = angularVelocity < 0 ? -50 : 50;
		else {
			if (angularVelocity > 0)
				angularVelocity = MAX(30, angularVelocity);
			if (angularVelocity < 0)
				angularVelocity = MIN(-30, angularVelocity);
		}
		if (get_full_ticks() - moveDelay < 1000) {
			reset();
			setM(3);
			setBearing(180);
			setW(angularVelocity);
			addComponent();
			parseWheelbaseValues();
		}

		if (get_full_ticks() - moveDelay > 1000) {
			if (pneumatics.P1 != false) {
				pneumatics.P1 = false;
				pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
			}
			if (get_full_ticks() - moveDelay > 1000
					&& get_full_ticks() - moveDelay < 1250) {
				motor_set_vel(MOTOR1, 2, CLOSE_LOOP);
				motor_set_vel(MOTOR2, 2, CLOSE_LOOP);
				motor_set_vel(MOTOR3, 2, CLOSE_LOOP);
			} else if (get_full_ticks() - moveDelay > 1250
					&& get_full_ticks() - moveDelay < 1500) {
				motor_set_vel(MOTOR1, -2, CLOSE_LOOP);
				motor_set_vel(MOTOR2, -2, CLOSE_LOOP);
				motor_set_vel(MOTOR3, -2, CLOSE_LOOP);
			} else if (get_full_ticks() - moveDelay > 1500) {
				motor_set_vel(MOTOR1, 0, OPEN_LOOP);
				motor_set_vel(MOTOR2, 0, OPEN_LOOP);
				motor_set_vel(MOTOR3, 0, OPEN_LOOP);
				if (pneumatics.P3 == false)
					climbingState = GRAB_PROPELLER;
				else {
					climbDelay = get_full_ticks();
					climbingState = CLIMB_POLE;
				}
			}
		}
	}

	// Move arm to correct position.
	if (climbingState == GRAB_PROPELLER) {
		int armError = get_encoder_value(MOTOR8)
				- (pneumatics.P3 == false ? 17000 : 75000);
		//if (prevLimitSwitch[3] == 1 && !limitSwitch[2]) {

		//		setM(0);
		//		setBearing(0);
		//		setW(0);
		//		addComponent();
		//		parseWheelbaseValues();

		if (!(Abs(armError) <= 2000))
			sendArmCommand(armError < 0 ? -60 : 60);
		else if (Abs(armError) <= 2000) {
			sendArmCommand(0);
			if (pneumatics.P3 != true && allow4thUpdate) {
				pneumatics.P3 = true;
				pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
			} else {
				climbingState = CLIMB_POLE;
				climbDelay = get_full_ticks();
			}
		}
	}

	if (climbingState == CLIMB_POLE && get_full_ticks() - climbDelay >= 500) {
		sendClimbCommand(1200);
		sendArmCommand(-60);
		climbingState = INSTALL_PROPELLER;
	}
	if (limitSwitch[2] && climbingState == INSTALL_PROPELLER) {
		sendClimbCommand(0);
		if (get_full_ticks() - waitDelay >= 500
				&& get_full_ticks() - waitDelay < 1200) {
			pneumatics.P2 = true;
			pneumatic_control(GPIOE, GPIO_Pin_13, pneumatics.P2);
		}
		if (get_full_ticks() - waitDelay >= 1200) {
			int armError = get_encoder_value(MOTOR8) - 25890;
			if (!(Abs(armError) <= 1000))
				sendArmCommand(armError < 0 ? -60 : 60);
			else if (Abs(armError) <= 1000) {
				sendArmCommand(0);
				pneumatics.P2 = false;
				pneumatic_control(GPIOE, GPIO_Pin_13, pneumatics.P2);
				climbingState = RESTORE;
			}
		}
	}
}

/**
 * @brief Arm Position Update, checks current IR sensor status relative to previous
 */

void armUpdate() {
	sendArmCommand(armIr == 0 ? -40 : 0);
	prevArmIr = armIr;
}

bool getLS(int index) {
	return limitSwitch[index];
}

int ctr = 1, retryDelay = 0;

bool retryIrChecking(void) {
	retryIr = gpio_read_input(&PE0);  //Replace With Proper Port
	//if(retryIr == 1 && prevRetryIr == 0) prevRetryIr =1;
	if (retryIr == 0) {
		//prevRetryIr = 0;
		if (ctr == 0) {
			ctr = 1;
			return false;
		} else if (ctr == 1) {
			ctr++;
			return true;
		} else {
			return true;
		}
	}
}

void retryProcedureCheck(void) {
	if (retryIrChecking()) {
		if (ctr == 2) {
			ctr++;
			//dequeue(getSize());
			setBrushlessMagnitude(0);
			retryDelay = get_full_ticks();
			lastWait = -1;
			currMode = WAITRETRY;

			CAN_MESSAGE txMsg;
			txMsg.id = 0x300;
			txMsg.length = 0;
			can_tx_enqueue(txMsg);
		} else if (ctr == 3) {
			if (get_full_ticks() - retryDelay > 7000) {
				queueTargetPoint(wagamama, wagateki, 175, 2000, 10, -1, 6500); //-200 //TEST FIELD -50 0
				ctr = 1;
				currMode = PIDMODE;
			} else {
				setBrushlessMagnitude(0);
				if (get_full_ticks() - retryDelay > 5000) {
					if (get_full_ticks() - retryDelay % 200 == 0) {
						CAN_MESSAGE txMsg;
						txMsg.id = 0x301;
						txMsg.length = 0;
						can_tx_enqueue(txMsg);
					}
				}
			}
		}
	}
}
