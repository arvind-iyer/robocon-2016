#ifndef	__ROBOCON_H
#define	__ROBOCON_H

#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"

/*** Essential ***/
#include "ticks.h"
#include "delay.h"

#include "gyro.h"
#include "gpio.h"
#include "buzzer.h"
#include "led.h"
#include "tft.h"
#include "interface.h"
#include "xbc_mb.h"
#include "button.h"
#include "encoder.h"
#include "ultrasonic_mb.h"
#include "laser_sensor.h"
#include "pk/pk_wheelbase.h"	//pk
#include "pk/hybrid_pneumatic.h"
#include "pk/hybrid_pneumatic.h"
#include "pk/hybrid_gpio.h"
#include "pk/control.h"

/*** Optional ***/
#include "can_protocol.h"
#include "usart.h"
#include "approx_math.h"
#include "servo.h"
#include "gyro.h"
#include "bluetooth.h"
#include "wheel_base.h"
#include "xbc_mb.h"
#include "log.h"
#include "nec_mb.h"
#include "flash.h"
#include "ultrasonic.h" 

void robocon_main(void);
void _updateScreen(void);
void controllerInputUpdate(void);

extern Pneumatics pneumatics;
extern bool prevLimitSwitch[4];
extern bool robotMode;
extern bool armIr;
extern bool manualMode;
extern bool laserAuto;
extern bool autoModeLaser;
extern bool autoPIDMode;

extern bool climbing;

#endif	/* __ROBOCON_H */
