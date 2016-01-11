#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "gpio.h"
#include "stm32f10x_tim.h"
#include "lcd_main.h"

/*****/
#define BUTTON_DOWN_LISTENER()    (button_pressed(BUTTON_JS_DOWN) == 1 || button_hold(BUTTON_JS_DOWN, 10, 3) || button_pressed(BUTTON_XBC_S) == 1 || button_hold(BUTTON_XBC_S, 10, 3))
#define BUTTON_UP_LISTENER()      (button_pressed(BUTTON_JS_UP) == 1 || button_hold(BUTTON_JS_UP, 10, 3) || button_pressed(BUTTON_XBC_N) == 1 || button_hold(BUTTON_XBC_N, 10, 3))
#define BUTTON_LEFT_LISTENER()    (button_pressed(BUTTON_JS_LEFT) == 1 || button_hold(BUTTON_JS_LEFT, 10, 3) || button_pressed(BUTTON_XBC_W) == 1 || button_hold(BUTTON_XBC_W, 10, 1))
#define BUTTON_RIGHT_LISTENER()   (button_pressed(BUTTON_JS_RIGHT) == 1 || button_hold(BUTTON_JS_RIGHT, 10, 3) || button_pressed(BUTTON_XBC_E) == 1 || button_hold(BUTTON_XBC_E, 10, 1))
#define BUTTON_ENTER_LISTENER()   (button_pressed(BUTTON_JS_CENTER) == 1 || button_pressed(BUTTON_XBC_START) == 1)
#define BUTTON_RETURN_LISTENER()   (button_pressed(BUTTON_1) == 5 || button_pressed(BUTTON_2) == 5 || button_pressed(BUTTON_XBC_BACK) == 5)

/****/

#define BUTTON_JS_UP_GPIO				((GPIO*) &PD0)
#define BUTTON_JS_LEFT_GPIO			((GPIO*) &PD1)
#define BUTTON_JS_DOWN_GPIO			((GPIO*) &PB8)
#define BUTTON_JS_RIGHT_GPIO		((GPIO*) &PD3)
#define BUTTON_JS_CENTER_GPIO		((GPIO*) &PD4)

#define	BUTTON_1_GPIO							((GPIO*) &PB6)
#define	BUTTON_2_GPIO							((GPIO*) &PB7)

#define BUTTON_RELEASED_LIMIT			20    /*!< Reset the button release time after the limit */

#define	BUTTON_COUNT							7    /*!< Number of buttons (exclude XBOX) */
#define XBC_BUTTON_START_ID       7
#define XBC_BUTTON_COUNTS         20


typedef enum {
	BUTTON_JS_UP 				= 0,
	BUTTON_JS_LEFT			= 1,
	BUTTON_JS_DOWN			= 2,
	BUTTON_JS_RIGHT			= 3,
	BUTTON_JS_CENTER 		= 4, 

	BUTTON_1 = 5,
	BUTTON_2 = 6,
 
  BUTTON_XBC_N	= XBC_BUTTON_START_ID,
  BUTTON_XBC_S 	= XBC_BUTTON_START_ID+1,
  BUTTON_XBC_W	= XBC_BUTTON_START_ID+2,
  BUTTON_XBC_E	= XBC_BUTTON_START_ID+3,
  
  BUTTON_XBC_START	= XBC_BUTTON_START_ID+4,
  BUTTON_XBC_BACK	= XBC_BUTTON_START_ID+5,
  BUTTON_XBC_L_JOY	= XBC_BUTTON_START_ID+6,
  BUTTON_XBC_R_JOY	= XBC_BUTTON_START_ID+7,
  
  BUTTON_XBC_LB	= XBC_BUTTON_START_ID+8,
  BUTTON_XBC_RB	= XBC_BUTTON_START_ID+9,
  BUTTON_XBC_XBOX	= XBC_BUTTON_START_ID+10,
  BUTTON_XBC_NULL	= XBC_BUTTON_START_ID+11,
  
  BUTTON_XBC_A	= XBC_BUTTON_START_ID+12,
  BUTTON_XBC_B	= XBC_BUTTON_START_ID+13,
  BUTTON_XBC_X	= XBC_BUTTON_START_ID+14,
  BUTTON_XBC_Y	= XBC_BUTTON_START_ID+15,
  
  BUTTON_XBC_NE	= XBC_BUTTON_START_ID+16,
  BUTTON_XBC_SE	= XBC_BUTTON_START_ID+17,
  BUTTON_XBC_SW	= XBC_BUTTON_START_ID+18,
  BUTTON_XBC_NW	= XBC_BUTTON_START_ID+19
  } BUTTON;

typedef enum {
	BUTTON_PRESSED			= 0,
	BUTTON_UNPRESSED 		= 1
} BUTTON_STATE;

void button_init(void);
void button_update(void);
u16 button_pressed(BUTTON b);
u8 button_hold(BUTTON b, u16 threshold, u8 mod);
u16 button_released(BUTTON b);


#endif /* __BUTTON_H */

