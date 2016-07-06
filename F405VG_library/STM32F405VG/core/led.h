#ifndef	__LED_H
#define	__LED_H

#include "stm32f4xx.h"
#include "gpio.h"

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

#define LED_1_GPIO PC4

#define LED_COUNT 1
typedef enum {
	LED_D1 = 1 << 0,	// 1 (00001)
} LED;
	
typedef enum {
	LED_OFF = Bit_RESET,
	LED_ON	= Bit_SET
} LED_STATE;

void led_init(void);
void led_control(LED led, LED_STATE state);
void led_blink(LED led);
u8 get_led_state(void);

#endif /* __LED_H */
