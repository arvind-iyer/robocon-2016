#include "mpu6050.h"

void mpu_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB1PeriphClockCmd(MPU_I2C_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(MPU_GPIO_RCC, ENABLE);
	
	//Init the SCL and SDA Pin
	GPIO_PinAFConfig(MPU_GPIO, MPU_SCL_PIN_SRC, MPU_AF_GPIO);
	GPIO_PinAFConfig(MPU_GPIO, MPU_SDA_PIN_SRC, MPU_AF_GPIO);
	
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
	
	I2C_Cmd(MPU_I2C, ENABLE);
}

void getRawAccelGyro(s16* AccelGyro){
    u8 tmpBuffer[14];
    mpu_buffer_read(MPU_ADDRESS, tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 14);
    /* Get acceleration */
    for (int i = 0; i < 3; i++)
        AccelGyro[i] = ((s16) ((u16) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
    /* Get Angular rate */
    for (int i = 4; i < 7; i++)
        AccelGyro[i - 1] = ((s16) ((u16) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
}

void mpu_write_bit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data){
    uint8_t tmp;
    mpu_buffer_read(slaveAddr, &tmp, regAddr, 1);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    mpu_write_byte(slaveAddr, &tmp, regAddr);
}


void mpu_wake_up(){
    mpu_write_bit(MPU_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, 0);//Disalbe it
}

void mpu_write_byte(u8 slaveAddr, u8* pBuffer, u8 writeAddr){
    // ENTR_CRT_SECTION();

    /* Send START condition */
    I2C_GenerateSTART(MPU_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send MPU6050 address for write */
    I2C_Send7bitAddress(MPU_I2C, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the MPU6050's internal address to write to */
    I2C_SendData(MPU_I2C, writeAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the byte to be written */
    I2C_SendData(MPU_I2C, *pBuffer);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(MPU_I2C, ENABLE);
}

void mpu_buffer_read(u8 deviceAdd, u8* buffer, u8 readAdd, u16 numOfByte){
	//wait until i2c is not busy
	while (I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY));
	//Generate start 
	I2C_GenerateSTART(MPU_I2C, ENABLE);
  //Test on EV5 and clear it
  while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//Send the target device address
	I2C_Send7bitAddress(MPU_I2C, deviceAdd, I2C_Direction_Transmitter);
	//Test on EV6 and clear it
  while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_Cmd(MPU_I2C, ENABLE);
	//Send the address to be read within MPU6050
	I2C_SendData(MPU_I2C, readAdd);
	//Test on EV8 and clear it
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	// Send STRAT condition again
  I2C_GenerateSTART(MPU_I2C, ENABLE);
	//Test on EV5 and clear it
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	//Send MPU6050 address for read
	I2C_Send7bitAddress(MPU_I2C, deviceAdd, I2C_Direction_Receiver);
	// Test on EV6 and clear it
	while (!I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	/* While there is data to be read */
	while (numOfByte)
	{
			if (numOfByte == 1)
			{
					/* Disable Acknowledgement */
					I2C_AcknowledgeConfig(MPU_I2C, DISABLE);

					/* Send STOP Condition */
					I2C_GenerateSTOP(MPU_I2C, ENABLE);
			}

			/* Test on EV7 and clear it */
			if (I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
			{
					/* Read a byte from the MPU6050 */
					*buffer = I2C_ReceiveData(MPU_I2C);

					/* Point to the next location where the byte read will be saved */
					buffer++;

					/* Decrement the read bytes counter */
					numOfByte--;
			}
	}

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(MPU_I2C, ENABLE);
}
