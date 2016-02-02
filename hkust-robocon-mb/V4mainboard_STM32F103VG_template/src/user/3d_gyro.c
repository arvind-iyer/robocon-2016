#include "3d_gyro.h"
u8 I2C_Rx_Buffer[14]; //It is empty!
u8 *I2C_pt = I2C_Rx_Buffer;
s16 AccelGyro[6];
s16 *Accelpt = AccelGyro;

void gyro3d_init()
{
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_I2C_Config;

	/* Enable I2C and GPIO clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure I2C pins: SCL and SDA */
	GPIO_I2C_Config.GPIO_Pin = MPU6050_I2C_SCL_Pin | MPU6050_I2C_SDA_Pin;
	GPIO_I2C_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_I2C_Config.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(MPU6050_I2C_Port, &GPIO_I2C_Config);

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = MPU6050_DEFAULT_ADDRESS; // MPU6050 7-bit adress = 0x68, 8-bit adress = 0xD0;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = MPU6050_I2C_Speed;

	/* Apply I2C configuration after enabling it */
	I2C_Init(MPU6050_I2C, &I2C_InitStructure);
	/* I2C Peripheral Enable */
	I2C_Cmd(MPU6050_I2C, ENABLE);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);//apply all setup	
	EXTI_ClearITPendingBit(EXTI_Line4);
	
	MPU6050_Initialize();
	gyro_dma_init();
}

void MPU6050_Initialize(void) 
{
MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 1<<7);//reset the whole module first

while(get_ticks()%50 != 0);	//wait for 50ms for the gyro to stable

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_CLOCK_PLL_ZGYRO);//PLL with Z axis gyroscope reference
	
MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_CONFIG, 0x01);		//DLPF_CFG = 1: Fs=1khz; bandwidth=42hz	
    	
MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0x01);	//500Hz sample rate ~ 2ms

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, MPU6050_GYRO_FS_2000);	//Gyro full scale setting

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACCEL_FS_16);	//Accel full scale setting

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 1<<4);		//interrupt status bits are cleared on any read operation

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_ENABLE, 1<<0);		//interupt occurs when data is ready. The interupt routine is in the receiver.c file.	

MPU6050_Write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 0x07);//reset gyro and accel sensor
}

void MPU6050_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t data) 
{
    uint8_t tmp;  
    tmp = data;
    MPU6050_I2C_ByteWrite(slaveAddr,&tmp,regAddr);   
}
//------------------------------------------------------------------
void MPU6050_I2C_ByteWrite(u8 slaveAddr, u8* pBuffer, u8 writeAddr)
{

/* Send START condition */
I2C_GenerateSTART(MPU6050_I2C, ENABLE);
/* Test on EV5 and clear it */
while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));
/* Send MPU6050 address for write */
I2C_Send7bitAddress(MPU6050_I2C, slaveAddr, I2C_Direction_Transmitter);
/* Test on EV6 and clear it */
while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
/* Send the MPU6050's internal address to write to */
I2C_SendData(MPU6050_I2C, writeAddr);
/* Test on EV8 and clear it */
//while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
/* Send the byte to be written */
if (pBuffer!=0) I2C_SendData(MPU6050_I2C, *pBuffer);
/* Test on EV8_2 and clear it */
while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
/* Send STOP condition */
I2C_GenerateSTOP(MPU6050_I2C, ENABLE);

}

void gyro_dma_init()
{
		NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;

	DMA_DeInit(MPU6050_DMA_Channel); //reset DMA1 channe1 to default values;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C_DR_Address; //=0x40005410 : address of data reading register of I2C1
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2C_Rx_Buffer; //variable to store data
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //channel will be used for peripheral to memory transfer
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	//setting normal mode (non circular)
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	//medium priority
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//Location assigned to peripheral register will be source
	DMA_InitStructure.DMA_BufferSize = 14;	//number of data to be transfered
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//automatic memory increment enable for memory
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//source peripheral data size = 8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//destination memory data size = 8bit
	DMA_Init(MPU6050_DMA_Channel, &DMA_InitStructure);
	DMA_ITConfig(MPU6050_DMA_Channel, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn; //I2C1 connect to channel 7 of DMA1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(MPU6050_INT_Exti))			//MPU6050_INT
	{
	EXTI_ClearITPendingBit(MPU6050_INT_Exti);
	//#ifndef USE_I2C_DMA
	//Prepare_Gyro_Data();	//Read out the accel and gyro data whenever interrupt occurs.
	//#else
	I2C_DMA_Read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H);
	//#endif
	}
}

void I2C_DMA_Read(u8 slaveAddr, u8 readAddr)
{
	/* Disable DMA channel*/
	DMA_Cmd(MPU6050_DMA_Channel, DISABLE);
	/* Set current data number again to 14 for MPu6050, only possible after disabling the DMA channel */
	DMA_SetCurrDataCounter(MPU6050_DMA_Channel, 14);

	/* While the bus is busy */
	while(I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BUSY));

	/* Enable DMA NACK automatic generation */
	I2C_DMALastTransferCmd(MPU6050_I2C, ENABLE);					//Note this one, very important

	/* Send START condition */
	I2C_GenerateSTART(MPU6050_I2C, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send MPU6050 address for write */
	I2C_Send7bitAddress(MPU6050_I2C, slaveAddr, I2C_Direction_Transmitter); 

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Clear EV6 by setting again the PE bit */
	I2C_Cmd(MPU6050_I2C, ENABLE);

	/* Send the MPU6050's internal address to write to */
	I2C_SendData(MPU6050_I2C, readAddr);

	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send STRAT condition a second time */
	I2C_GenerateSTART(MPU6050_I2C, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send MPU6050 address for read */
	I2C_Send7bitAddress(MPU6050_I2C, slaveAddr, I2C_Direction_Receiver);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	/* Start DMA to receive data from I2C */
	DMA_Cmd(MPU6050_DMA_Channel, ENABLE);
	I2C_DMACmd(MPU6050_I2C, ENABLE);

	// When the data transmission is complete, it will automatically jump to DMA interrupt routine to finish the rest.
	//now go back to the main routine
}

void DMA1_Channel7_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
	{
	/* Clear transmission complete flag */
	DMA_ClearFlag(DMA1_FLAG_TC7);

	I2C_DMACmd(MPU6050_I2C, DISABLE);
	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
	/* Disable DMA channel*/
	DMA_Cmd(MPU6050_DMA_Channel, DISABLE);

	//Read Accel data from byte 0 to byte 2
	for(int i=0; i<3; i++) 
	AccelGyro[i]=((s16)((u16)I2C_Rx_Buffer[2*i] << 8) + I2C_Rx_Buffer[2*i+1]);
	//Skip byte 3 of temperature data
	//Read Gyro data from byte 4 to byte 6
	for(int i=4; i<7; i++)
	AccelGyro[i-1]=((s16)((u16)I2C_Rx_Buffer[2*i] << 8) + I2C_Rx_Buffer[2*i+1]);
	}
}