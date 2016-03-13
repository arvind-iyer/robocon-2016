#ifndef PCA9685_H
#define PCA9685_H

#include <stm32f10x_i2c.h>

//7-bit addresses
#define MASTER_ADDRESS		0x1C << 1	// =0011100X
#define SLAVE_ADDRESS			0x81 //<< 1	// =1000000X

//internal clock = 100khz
#define INTERNAL_CLK			100000	

//register values
#define MODE1					0x00
#define LED14_ON_L		0x3E

//MODE1 flag bit
#define MODE1_AI			0x05
#define MODE1_ALLCALL	0x00

void i2c_init();
u8 pca9685_init();
u8 pca9685_set_pwm();

#endif