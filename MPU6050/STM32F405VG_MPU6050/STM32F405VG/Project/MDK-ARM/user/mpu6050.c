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

void mpu_init(){
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
	I2C_InitStruct.I2C_ClockSpeed = 100000;
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
** @param buffer: The pointer to the buffer of size 6 in which raw data should be stored
**/
void getRawAccelGyro(){
    u8 tmpBuffer[14];
    mpu_buffer_read(tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 14);
    //Obtain acceleration
    for (u8 i = 0; i < 3; i++){
        IMU_Buffer[i] = ((s16) ((u16) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
		}
    //Obtain angular velocity
    for (u8 i = 4; i < 7; i++)
        IMU_Buffer[i - 1] = ((s16) ((u16) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
}

/**
** Write a single bit into the MPU6050, while maintaining the rest of the byte the same
** @param reg_add:		Address of the field to be edited in MPU6050's register
** @param bit_pos:		The position of the bit to be changed in the byte
** @param data:				The data to be written
**/
void mpu_write_bit(uint8_t reg_add, uint8_t bit_pos, uint8_t data){
	uint8_t tmp;
	//First get the original byte
  mpu_buffer_read(&tmp, reg_add, 1);
	//Then modify the original byte and send the new byte
  tmp = (data != 0) ? (tmp | (1 << bit_pos)) : (tmp & ~(1 << bit_pos));
  mpu_write_byte(&tmp, reg_add);
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
