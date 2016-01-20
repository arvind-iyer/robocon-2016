#ifndef	__LED_H
#define	__LED_H

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "gpio.h"
#include "stm32f10x_tim.h"

#define	LED_RED_GPIO				((GPIO*) &PB10)
#define	LED_BLUE_GPIO				((GPIO*) &PB11)
#define	LED_GREEN_GPIO				((GPIO*) &PB12)

typedef enum {
	LED_RED = 1 << 0,	// 1 (001)
	LED_BLUE = 1 << 1,	// 2 (010)
	LED_GREEN = 1 << 2,		// 4 (100)
} LED;
	
typedef enum {
	LED_OFF = 0,
	LED_ON	= 1
} LED_STATE;

void led_init(void);
void led_control(LED led, LED_STATE state);
void led_blink(LED led);
u8 get_led_state();

#endif /* __LED_H */
