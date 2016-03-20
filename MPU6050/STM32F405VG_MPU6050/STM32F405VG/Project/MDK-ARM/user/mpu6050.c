/******************************************************
** MPU6050 Program - STM32F405VG
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** March 2016
*******************************************************/

#include "mpu6050.h"

s16 IMU_Buffer[6]={0x00};

/**
** A helper function to init the I2C and the range setting
** Also check whethere all setting have been correctly set
** @return is operation successful?
**/
bool mpu_init(){
	mpu_i2c_init();
	mpu_wake_up();
	mpu_set_max_accel(ACCEL_RANGE);
	mpu_set_max_angular_vel(GYRO_RANGE);
	if (mpu_get_max_angular_vel() != GYRO_RANGE || mpu_get_max_accel() !=ACCEL_RANGE){
		return false;
	}else{
		return true;
	}
}

void mpu_i2c_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB1PeriphClockCmd(MPU_I2C_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(MPU_GPIO_RCC, ENABLE);
	
	//Remap the pins to SCL and SDA
	GPIO_PinAFConfig(MPU_GPIO, MPU_SCL_PIN_SRC, MPU_AF_GPIO);
	GPIO_PinAFConfig(MPU_GPIO, MPU_SDA_PIN_SRC, MPU_AF_GPIO);
	
	//Init the SCL and SDA Pin
	GPIO_InitStruct.GPIO_Pin = MPU_SCL_PIN | MPU_SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(MPU_GPIO, &GPIO_InitStruct);
	
	//Init the I2C
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_ClockSpeed = 100000; //Common speed
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; 
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_OwnAddress1 = MPU_ADDRESS;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(MPU_I2C, &I2C_InitStruct);
	
	//Start the I2C
	I2C_Cmd(MPU_I2C, ENABLE);
}

/**
** Get the raw acceleration and angular velocity from the MPU6050
** Stored in IMU_Buffer[6]
**/
void getRawAccelGyro(){
	u8 buffer[14];
	mpu_buffer_read(buffer, MPU6050_RA_ACCEL_XOUT_H, 14);
	
	//Obtain acceleration
	for (u8 i = 0; i < 3; i++){
		IMU_Buffer[i] = (s16) ((buffer[2 * i] << 8) | buffer[2 * i + 1]);
	}
	//Obtain angular velocity
	for (u8 i = 4; i < 7; i++){
		IMU_Buffer[i - 1] = (s16) ((buffer[2 * i] << 8) | buffer[2 * i + 1]);
	}
}

/**
** Set the max. acceleration the device can detect
** Due to data size(16-bit), higher max. value means smaller resolution
** @param range New full-scale accelerometer range setting
** 0 -> +- 2g, resolution = 2^16/4g
** 1 -> +- 4g, resolution = 2^16/8g
** 2 -> +- 8g, resolution = 2^16/16g
** 3 -> +- 16g, resolution = 2^16/32g
**/
void mpu_set_max_accel(uint8_t range){
	mpu_write_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

u8 mpu_get_max_accel(){
	u8 tmp;
	mpu_read_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, &tmp);
	return tmp;
}

/**
** Set the max. angular velocity the device can detect
** Due to data size(16-bit), higher max. value means smaller resolution
** @param range New full-scale accelerometer range setting
** 0 -> +- 250 deg/s, resolution = 2^16/500
** 1 -> +- 500 deg/s, resolution = 2^16/1000
** 2 -> +- 1000 deg/s, resolution = 2^16/2000
** 3 -> +- 2000 deg/s, resolution = 2^16/4000
**/
void mpu_set_max_angular_vel(uint8_t range){
	mpu_write_bits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

u8 mpu_get_max_angular_vel(){
	u8 tmp;
	mpu_read_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, &tmp);
	return tmp;
}

/**
** Write a single bit into the MPU6050, while maintaining the rest of the byte the same
** @param reg_add:		Address of the field to be edited in MPU6050's register
** @param bit_pos:		The position of the bit to be changed in the byte
** @param data:				The data to be written
**/
void mpu_write_bit(u8 reg_add, u8 bit_pos, u8 data){
	uint8_t source;
	
	//First get the original byte
  mpu_buffer_read(&source, reg_add, 1);
	//Then modify the original byte and send the new byte
  source = (data != 0) ? (source | (1 << bit_pos)) : (source & ~(1 << bit_pos));
  mpu_write_byte(&source, reg_add);
}

/**
** Write a single bit into the MPU6050, while maintaining the rest of the byte the same
** @param reg_add:		Address of the field to be edited in MPU6050's register
** @param bit_pos:		The position of the starting bit to be changed in the byte
** @param bit_len:		Number of the bits to be changed
** @param data:				The data to be written
**/
void mpu_write_bits(u8 reg_add, u8 bit_pos, u8 bit_len, u8 data){
	u8 source;

	//First get the original byte
	mpu_buffer_read(&source, reg_add, 1);

	//Create a mask to map the byte, by shifting a 1 by bit_len, like 0001000
	//Then minus one to turn all bits from the rightmost bit to the '1' bit to be 1, like 0000111
	//Finally shift this bunch of 1s to the correct position, like 00111000
	uint8_t mask = ((1 << bit_len) - 1) << (bit_pos - bit_len + 1); 

	data <<= (bit_pos - bit_len + 1); //Shift the data bits to match that of the mask
	data &= mask; //Make sure data only contains bits allowed by the mask, i.e. within length
	source &= ~mask; //Clear all the bits needed to be written
	source |= data; //Combine the source and the data
	//source |= (source&~mask)|(data&mask);
	mpu_write_byte(&source, reg_add); //Write it back into the device
}

/**
** Wake up the IMU.
** I'm awake! I'm awake! ~Druid of the Claw
**/
void mpu_wake_up(){
    mpu_write_bit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, 0);
}

/**
** Write a byte into the MPU6050
** @param data:				Data to be written
** @param reg_add:		Address of the field to be edited in MPU6050's register
**/
void mpu_write_byte(u8* data, u8 reg_add){
	I2C_GenerateSTART(MPU_I2C, ENABLE);//Start I2C transaction

	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));//Wait for Event to finish

	I2C_Send7bitAddress(MPU_I2C, MPU_ADDRESS, I2C_Direction_Transmitter);//Send the target device address
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//Wait for Event to finish

	I2C_SendData(MPU_I2C, reg_add);//Send the target data address inside target device
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//Wait for Event to finish

	I2C_SendData(MPU_I2C, *data);//Send the actual data
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//Wait for Event to finish

	I2C_GenerateSTOP(MPU_I2C, ENABLE);//Stop this transaction
}

/**
** Read mulitple bits from the MPU6050, while ignoring the rest of the byte
** @param reg_add:		Address of the field to be read in MPU6050's register
** @param bit_pos:		The position of the starting bit to be read in the byte
** @param bit_len:		Number of the bits to be read
** @param data:				The container to store the data
**/
void mpu_read_bits(u8 reg_add, u8 bit_pos, u8 bit_len, u8 *data){
	//See mpu_write_bits for bitwise explanation
	u8 tmp;
	mpu_buffer_read(&tmp, reg_add, 1);
	uint8_t mask = ((1 << bit_len) - 1) << (bit_pos - bit_len + 1);
	tmp &= mask;
	tmp >>= (bit_pos - bit_len + 1);
	*data = tmp;
}

/**
** Write a byte into the MPU6050
** @param buffer:			A buffer used to stored read data
** @param reg_add:		Address of the field to be edited in MPU6050's register
** @param length:			Length of data to be read
**/
void mpu_buffer_read(u8* buffer, u8 read_add, u16 length){
	while (I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY));//Wait until I2C is free
	
	I2C_GenerateSTART(MPU_I2C, ENABLE);//Start I2C transaction
  while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));//Wait for Event to finish
	I2C_Send7bitAddress(MPU_I2C, MPU_ADDRESS, I2C_Direction_Transmitter);//Send the target device address
	
  while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//Wait for Event to finish
	//I2C_Cmd(MPU_I2C, ENABLE); //This line no need(hopefully)
	
	I2C_SendData(MPU_I2C, read_add);//Send the address to be read within MPU6050
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//Wait for Event to finish
	
  I2C_GenerateSTART(MPU_I2C, ENABLE);//Send start again
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));//Wait for Event to finish
	
	I2C_Send7bitAddress(MPU_I2C, MPU_ADDRESS, I2C_Direction_Receiver);//Send MPU6050 address for read
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//Wait for Event to finish
	
	while (length>0){
		if (length == 1){
			I2C_AcknowledgeConfig(MPU_I2C, DISABLE);//disable acknowlegement
			I2C_GenerateSTOP(MPU_I2C, ENABLE);//Stop this transaction
		}
		if (I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)){//Wait for Event to finish
			*buffer++ = I2C_ReceiveData(MPU_I2C);
			length--;
		}
	}

	// Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(MPU_I2C, ENABLE);
}
