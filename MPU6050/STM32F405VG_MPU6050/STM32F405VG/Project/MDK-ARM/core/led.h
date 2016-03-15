#ifndef	__LED_H
#define	__LED_H

#include "stm32f4xx.h"
#include "tm_stm32f4_gpio.h"

//To display a byte in binary format, use println(BYTETOBINARYPATTERN, BYTETOBINARY(get_led_state()));
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


#define LED_1  GPIO_Pin_2
#define LED_2  GPIO_Pin_3
#define LED_3  GPIO_Pin_4
#define LED_4  GPIO_Pin_5
#define LED_5  GPIO_Pin_6
#define LED_GPIO GPIOE

#define LED_COUNT 5
typedef enum {
	LED_D1 = 1 << 0,	// 1 (00001)
	LED_D2 = 1 << 1,	// 2 (00010)
	LED_D3 = 1 << 2,	// 4 (00100)
	LED_D4 = 1 << 3,	// 8 (01000)
	LED_D5 = 1 << 4,	// 16 (10000)
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
