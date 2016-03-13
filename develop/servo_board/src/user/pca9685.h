#ifndef PCA9685_H
#define PCA9685_H

#include <stm32f10x_i2c.h>

//7-bit addresses
#define MASTER_ADDRESS		0x1C << 1	// =0011100X
#define SLAVE_ADDRESS			0x40 << 1	// =1000000X

//internal clock = 100khz
#define INTERNAL_CLK			100000	

void pca9685_init();
u8 pca9685_set_pwm(u8 reg, u8 val);

#endif