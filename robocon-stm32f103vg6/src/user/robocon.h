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

#include "buzzer.h"
#include "led.h"
#include "tft.h"
#include "interface.h"
#include "xbc_mb.h"
#include "button.h"
#include "encoder.h"
#include "ultrasonic_mb.h"
#include "pk/hybrid_pneumatic.h"

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

#endif	/* __ROBOCON_H */
