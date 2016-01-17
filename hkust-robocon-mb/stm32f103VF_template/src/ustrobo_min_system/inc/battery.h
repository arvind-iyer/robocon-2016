#ifndef __BATTERY_H
#define __BATTERY_H

/*
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include <string.h>
//#include "delay.h"

#define SAMPLE_BATTERY_VOLTAGE_1 		1200 	// Voltage of first source
#define SAMPLE_BATTERY_ADC_1 				3620	// Value of "adc_value" for above voltage
#define SAMPLE_BATTERY_VOLTAGE_2		1100 	// Voltage of second source
#define SAMPLE_BATTERY_ADC_2	 			3310	// Value of "adc_value" for above voltage

#define	BATTERY_USB_LEVEL						600		// <= 6.00V will be treated as USB Power
#define	BATTERY_LOW_LEVEL						1160	// <= 11.50V will generate warning sound
#define	BATTERY_SUPER_LOW_LEVEL			1120	// <= 11.10V will stop the program (while loop)
<<<<<<< HEAD
#define BATTERY_VALUE_COUNT				128


typedef enum {
	BATTERY_OKAY,
	BATTERY_LOW,
	BATTERY_SUPER_LOW,
	BATTERY_USB
} BATTERY_CHECK_RESULT;

void battery_adc_gpio_init(void);
void battery_adc_init(void);
void battery_adc_update(void);
u16 get_voltage(void);
u16 get_voltage_avg(void);
u16 get_battery_adc(void);
char* get_voltage_string(void);
BATTERY_CHECK_RESULT battery_check(void);
=======


typedef enum {
	BATTERY_OKAY,
	BATTERY_LOW,
	BATTERY_SUPER_LOW,
	BATTERY_USB
} BATTERY_CHECK_RESULT;

void battery_adc_gpio_init(void);
void battery_adc_init(void);
void battery_adc_update(void);
u16 get_voltage(void);
u16 get_voltage_avg(void);
u16 get_battery_adc(void);
char* get_voltage_string(void);
BATTERY_CHECK_RESULT battery_check(void);

*/
>>>>>>> 21fb6016ab8b7430bde8ddb725a254d7fc5f5776

#endif /* __BATTERY_H */
