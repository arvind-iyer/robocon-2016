#define	SlaveAddressMPU   0x68	  //��������5883��IIC�����еĴӵ�ַ

typedef unsigned char  uchar;

extern int accX, accY, accZ;
extern int gyroX, gyroY, gyroZ;
extern uchar	SlaveAddress;   	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
extern uchar Single_ReadI2C(uchar REG_Address);						//��ȡI2C����
extern void  Single_WriteI2C(uchar REG_Address,uchar REG_data);		//��I2Cд������

//****************************************
// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I	    0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	MPU6050_Addr	0xD0	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
//**************************************
//��ʼ��MPU6050
//**************************************
void InitMPU6050()
{
	SlaveAddress=MPU6050_Addr;
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//�������״̬
	Single_WriteI2C(SMPLRT_DIV, 0x07);// Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
	Single_WriteI2C(CONFIG, 0x00);// Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
	Single_WriteI2C(GYRO_CONFIG, 0x00);// Set Gyro Full Scale Range to ��250deg/s
	Single_WriteI2C(ACCEL_CONFIG, 0x00);// Set Accelerometer Full Scale Range to ��2g
	Single_WriteI2C(PWR_MGMT_1, 0x01);// PLL with X axis gyroscope reference and disable sleep mode
}
//**************************************
//// Get accelerometer and gyroscope values
//**************************************
void updateMPU6050()
{
	SlaveAddress=MPU6050_Addr;// Get accelerometer and gyroscope values

	accX=((Single_ReadI2C(ACCEL_XOUT_H)<<8)+Single_ReadI2C(ACCEL_XOUT_L));
	accY=-((Single_ReadI2C(ACCEL_YOUT_H)<<8)+Single_ReadI2C(ACCEL_YOUT_L));
	accZ=((Single_ReadI2C(ACCEL_ZOUT_H)<<8)+Single_ReadI2C(ACCEL_ZOUT_L));
	
	gyroX=-((Single_ReadI2C(GYRO_XOUT_H)<<8)+Single_ReadI2C(GYRO_XOUT_L));
	gyroY=((Single_ReadI2C(GYRO_YOUT_H)<<8)+Single_ReadI2C(GYRO_YOUT_L));
	gyroZ=-((Single_ReadI2C(GYRO_ZOUT_H)<<8)+Single_ReadI2C(GYRO_ZOUT_L));	
}
