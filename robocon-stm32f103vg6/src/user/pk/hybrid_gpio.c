#include "hybrid_gpio.h"
#include "pneumatic.h"
#include "gpio.h"
#include "ticks.h"
#include "can_motor.h"
#include "pk_wheelbase.h"
#include "pk_arm.h"
#include "pk_climb.h"
#include "robocon.h"
#include "ticks.h"

bool limitSwitch[4] = {false, false, false, false};
bool prevLimitSwitch[4] = {false, false, false, false};
bool armIr = false, prevArmIr = false, readyToClimb = false;
int climbDelay = 0;

/**
  * @brief Initializes the Hybrid's GPIO ports and some required variables
   */

void hybridGPIOInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// Limit switch GPIO initialization.
	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE9, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE11, GPIO_Speed_50MHz,GPIO_Mode_IPU, 1);
	// IR Sensor GPIO initialization.
	gpio_init(&PE8, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
}

bool armDir = false, fixingArm = false, climbLimit = false; // False = down, True = up
long lastLimitCheck = 0;

/**
  * @brief Checks all the limit switches on the hybrid
   */

void limitSwitchCheck() {
  limitSwitch[0] = !gpio_read_input(&PE6);
	limitSwitch[1]=  !gpio_read_input(&PE7);
	limitSwitch[2] = !gpio_read_input(&PE9);
	limitSwitch[3] = !gpio_read_input(&PE11);
	if (get_full_ticks() - lastLimitCheck >= 100 && (fixingArm)) {
		fixingArm = false;
		climbLimit  = false;
		motor_set_vel (MOTOR8, 0, CLOSE_LOOP);
		lastLimitCheck = get_full_ticks();
	}
	prevLimitSwitch[0] = (prevLimitSwitch[0] != limitSwitch[0] ? limitSwitch[0] : prevLimitSwitch[0]);
		prevLimitSwitch[1] = (prevLimitSwitch[1] != limitSwitch[1] ? limitSwitch[1] : prevLimitSwitch[1]);
	if (limitSwitch[0] || limitSwitch[1]) {
		
		armDir = limitSwitch[1] ? 1 : 0;
		motor_set_vel(MOTOR8, armDir ? -10 : 10, CLOSE_LOOP);
		fixingArm = true;
		lastLimitCheck = get_full_ticks();
		// If fixing arm, always make sure IR never gets triggered until fixingArm is false.
	}
	 if(prevLimitSwitch[2] != limitSwitch[2]) {
		prevLimitSwitch[2] = limitSwitch[2];
		sendClimbCommand(0);
		//climbLimit = true;	
		//lastLimitCheck = get_full_ticks();
	}
	if(prevLimitSwitch[3] != limitSwitch[3]){
		prevLimitSwitch[3] = limitSwitch[3];
		//sendWheelBaseMotorCommands(0,0,0);
		wheelbaseLock();
		laserAuto = false;
		manualMode = true;
		//pneumatics.P1 = false;
	}
	
	// Move arm to correct position.
	int armError = get_encoder_value(MOTOR8) - (pneumatics.P1 == false ? 80000 : 42928);
	if (prevLimitSwitch[3] == 1 && !climbing) {
		if(pneumatics.P1 != false) {
			pneumatics.P1 = false;
			_delay_ms(20);
			pneumatic_control(GPIOE, GPIO_Pin_15, pneumatics.P1);
			_delay_ms(100);
			motor_set_vel(MOTOR1, 0, OPEN_LOOP);
			motor_set_vel(MOTOR2, 0, OPEN_LOOP);
			motor_set_vel(MOTOR3, 0, OPEN_LOOP);
		}
		if (!(Abs(armError) <= 1000))
			sendArmCommand(armError < 0 ? -40 : 40);
		else if (Abs(armError) <= 1000) {
			sendArmCommand(0);
			if (pneumatics.P3 != true) {
				pneumatics.P3 = true;
				pneumatic_control(GPIOE, GPIO_Pin_14, pneumatics.P3);
			} else {
				climbing = true;
				climbDelay = get_full_ticks();
			}
		}
	}
	
	if (climbing && climbDelay - get_full_ticks() >= 500) {
		// start climbing motors.
	}
}

/**
  * @brief Arm Position Update, checks current IR sensor status relative to previous
   */

void armUpdate() {
	if(prevArmIr != armIr) {
		sendArmCommand(armIr == 0 ? -40 : 0);
		prevArmIr = armIr;
	}
}

bool getLS(int index) {
	return limitSwitch[index];
}
