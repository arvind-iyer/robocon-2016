#define	SlaveAddressMPU   0x68	  //定义器件5883在IIC总线中的从地址

typedef unsigned char  uchar;

extern int accX, accY, accZ;
extern int gyroX, gyroY, gyroZ;
extern uchar	SlaveAddress;   	//IIC写入时的地址字节数据，+1为读取
extern uchar Single_ReadI2C(uchar REG_Address);						//读取I2C数据
extern void  Single_WriteI2C(uchar REG_Address,uchar REG_data);		//向I2C写入数据

//****************************************
// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
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
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I	    0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	MPU6050_Addr	0xD0	//IIC写入时的地址字节数据，+1为读取
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{
	SlaveAddress=MPU6050_Addr;
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);// Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
	Single_WriteI2C(CONFIG, 0x00);// Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
	Single_WriteI2C(GYRO_CONFIG, 0x00);// Set Gyro Full Scale Range to ±250deg/s
	Single_WriteI2C(ACCEL_CONFIG, 0x00);// Set Accelerometer Full Scale Range to ±2g
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
