/**
  ******************************************************************************
  * @file    main.c
  * @author  William LEE
  * @date    09-June-2015
  * @brief   This file run the main part of motor driver.
  ******************************************************************************
  * @attention
  *
  * C99 mode is enabled, please ensure your keil support it or error will caused.
	* Change the motor_id in can_motor.h for different motor.
	* Uncomment the DEBUG_MODE definition to enable debugging mode.
	* Systicks is used in stm32f10x_it.c, for high frequency control (5000Hz)
  ******************************************************************************
  */
#include "main.h"
#define DEBUG_MODE

static u16 ticks_img = 65535;	//trivial value

int main(void)
{
	/** initialization **/
motor_init();
	ticks_init();
	/** end of init **/
	u8 state = 0;
	u16 last_seconds = 0;
	while (1) {
			
#ifdef DEBUG_MODE	// In debug mode, for hardware to debug by themselves
			//debug();
		
		u16 this_seconds = get_seconds();
		motor_control(1, 50);
//		if (this_seconds%3==0 && last_seconds!=this_seconds){
//			state==0?motor_control(1, 50):motor_control(0, 75);
//			state = !state;
//			last_seconds = this_seconds;
//		}
			life_signal();
#else							// Normal execute mode, led show life signal.
			/** flahsing led light to show mcu still working **/
			if (!is_encoder_working()) {
        encoder_malfunction_warning_signal();
      } else if (is_overspeed()) {
				motor_overspeed_signal();
			} else {
				life_signal();
			}
#endif
		}
}

