#include "stm32f10x.h"
#include "Kalman.h" 
#include <math.h>
#define RESTRICT_PITCH // Comment out to restrict roll to ��90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

struct Kalman kalmanX, kalmanY, kalmanZ; // Create the Kalman instances

/* IMU Data MPU6050 AND HMC5883 Data*/
int accX, accY, accZ;
int gyroX, gyroY, gyroZ;
int magX, magY, magZ;


double roll, pitch, yaw; // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer

double gyroXangle, gyroYangle, gyroZangle; // Angle calculate using the gyro only ֻ�������Ǽ���Ƕ�
double compAngleX, compAngleY, compAngleZ; // Calculated angle using a complementary filter  �õ�żƼ���Ƕ�
double kalAngleX, kalAngleY, kalAngleZ; // Calculated angle using a Kalman filter	��kalman����Ƕ�

//uint32_t timer,micros; //��һ��ʱ���뵱ǰʱ��
uint8_t i2cData[14]; // Buffer for I2C data

#define MAG0MAX 603
#define MAG0MIN -578

#define MAG1MAX 542
#define MAG1MIN -701

#define MAG2MAX 547
#define MAG2MIN -556

#define RAD_TO_DEG 57.295779513082320876798154814105  // ����ת�Ƕȵ�ת����
#define DEG_TO_RAD 0.01745329251994329576923690768489 // �Ƕ�ת���ȵ�ת����

float magOffset[3] = { (MAG0MAX + MAG0MIN) / 2, (MAG1MAX + MAG1MIN) / 2, (MAG2MAX + MAG2MIN) / 2 };
double magGain[3];

void  SYSTICK_Configuration(void);								//ϵͳ�δ��ж�����
void  RCC_Configuration(void);
void  updatePitchRoll(void);									//���ݼ��ټ�ˢ��Pitch��Roll����
void  updateYaw(void);											//���ݴ�����ˢ��Yaw��
void  InitAll(void);											//ѭ��ǰ�ĳ�ʼ��
void  func(void);												//ѭ���������
extern void InitMPU6050(void);									//��ʼ��MPU6050
extern void InitHMC5883(void);									//��ʼ��HMC5883
extern void updateMPU6050(void);								//Get accelerometer and gyroscope values
extern void updateHMC5883(void);								//Get magnetometer values
extern void USART1_Configuration(void);							//���ڳ�ʼ��
extern void USART1_SendData(u8 SendData);						//���ڷ��ͺ���
extern void I2C_GPIO_Config(void);								//I2C��ʼ������
/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ�������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
int main(void)
{
  	RCC_Configuration();   				//ϵͳʱ������	
  	USART1_Configuration();
  	I2C_GPIO_Config();
  	InitHMC5883();
	InitMPU6050();
	InitAll();	
//	SYSTICK_Configuration();				
 	while(1)
	{
		func();
  	}
}
///*
//ϵͳ�δ��ж�����
//*/
//void SYSTICK_Configuration(void)
//{
//	micros=0;//ȫ�ּ���ʱ�����
// 	if (SysTick_Config(72000))		    //ʱ�ӽ����ж�ʱ1000msһ��  ���ڶ�ʱ 
//   	{ 
//	    /* Capture error */ 
////	    while (1);
//   	}
//}
///*
//��ǰʱ��++.Ϊ�˷�ֹ����������2^20ʱ���������
//*/
//void SysTickHandler(void)
//{
// 	micros++;
//	if(micros>(1<<20))
//		  micros=0;
//}
/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void)
{   
	SystemInit();
}

void InitAll()
{
	/* Set Kalman and gyro starting angle */
	updateMPU6050();
	updateHMC5883();
	updatePitchRoll();
	updateYaw();
	
	setAngle(&kalmanX,roll); // First set roll starting angle
	gyroXangle = roll;
	compAngleX = roll;
	
	setAngle(&kalmanY,pitch); // Then pitch
	gyroYangle = pitch;
	compAngleY = pitch;
	
	setAngle(&kalmanZ,yaw); // And finally yaw
	gyroZangle = yaw;
	compAngleZ = yaw;
	
//	timer = micros; // Initialize the timer	
}

void send(double xx,double yy,double zz)
{
	int	a[3];
 	u8 i,sendData[12];	   
	a[0]=(int)xx;a[1]=(int)yy;a[2]=(int)zz;
	for(i=0;i<3;i++)
	{
		if(a[i]<0){
			sendData[i*4]='-';
			a[i]=-a[i];
		}
		else sendData[i*4]=' ';
		sendData[i*4+1]=(u8)(a[i]%1000/100+0x30);
		sendData[i*4+2]=(u8)(a[i]%100/10+0x30);
		sendData[i*4+3]=(u8)(a[i]%10+0x30);
	}
	for(i=0;i<12;i++)
	{
		USART1_SendData(sendData[i]);
	}
	USART1_SendData(0x0D);
	USART1_SendData(0x0A);
}

void func()
{
	double gyroXrate,gyroYrate,gyroZrate,dt=0.01;
	/* Update all the IMU values */
	updateMPU6050();
	updateHMC5883();
	
//	dt = (double)(micros - timer) / 1000; // Calculate delta time
//	timer = micros;
//	if(dt<0)dt+=(1<<20);	//ʱ���������Եģ��п��ܵ�ǰʱ��С���ϴ�ʱ�䣬��Ϊ�������Զ�������λ���ʱ�䣬����������1<<20

	/* Roll and pitch estimation */
	updatePitchRoll(); 			//�òɼ��ļ��ټƵ�ֵ����roll��pitch��ֵ
	gyroXrate = gyroX / 131.0; 	// Convert to deg/s	�������ǵĽǼ��ٶȰ��յ����趨������ת��Ϊ��/s
	gyroYrate = gyroY / 131.0; 	// Convert to deg/s
	
	#ifdef RESTRICT_PITCH		//���������#define RESTRICT_PITCH�Ͳ������ַ������㣬��ֹ����-180��180֮�����Ծ
	// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
	if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
		setAngle(&kalmanX,roll);
		compAngleX = roll;
		kalAngleX = roll;
		gyroXangle = roll;
	} else
	kalAngleX = getAngle(&kalmanX, roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
	
	if (fabs(kalAngleX) > 90)
		gyroYrate = -gyroYrate; // Invert rate, so it fits the restricted accelerometer reading
	kalAngleY = getAngle(&kalmanY,pitch, gyroYrate, dt);
	#else
	// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
	if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
		kalmanY.setAngle(pitch);
		compAngleY = pitch;
		kalAngleY = pitch;
		gyroYangle = pitch;
	} else
	kalAngleY = getAngle(&kalmanY, pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter
	
	if (abs(kalAngleY) > 90)
		gyroXrate = -gyroXrate; // Invert rate, so it fits the restricted accelerometer reading
	kalAngleX = getAngle(&kalmanX, roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
	#endif
	
	
	/* Yaw estimation */
	updateYaw();
	gyroZrate = gyroZ / 131.0; // Convert to deg/s
	// This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
	if ((yaw < -90 && kalAngleZ > 90) || (yaw > 90 && kalAngleZ < -90)) {
		setAngle(&kalmanZ,yaw);
		compAngleZ = yaw;
		kalAngleZ = yaw;
		gyroZangle = yaw;
	} else
	kalAngleZ = getAngle(&kalmanZ, yaw, gyroZrate, dt); // Calculate the angle using a Kalman filter
	
	
	/* Estimate angles using gyro only */
	gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
	gyroYangle += gyroYrate * dt;
	gyroZangle += gyroZrate * dt;
	//gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate from the Kalman filter
	//gyroYangle += kalmanY.getRate() * dt;
	//gyroZangle += kalmanZ.getRate() * dt;
	
	/* Estimate angles using complimentary filter */
	compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
	compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
	compAngleZ = 0.93 * (compAngleZ + gyroZrate * dt) + 0.07 * yaw;
	
	// Reset the gyro angles when they has drifted too much
	if (gyroXangle < -180 || gyroXangle > 180)
		gyroXangle = kalAngleX;
	if (gyroYangle < -180 || gyroYangle > 180)
		gyroYangle = kalAngleY;
	if (gyroZangle < -180 || gyroZangle > 180)
		gyroZangle = kalAngleZ;
	
	
	send(roll,pitch,yaw);
//	send(gyroXangle,gyroYangle,gyroZangle);
//	send(compAngleX,compAngleY,compAngleZ);
//	send(kalAngleX,kalAngleY,kalAngleZ);
//	send(kalAngleY,compAngleY,gyroYangle);
} 

//****************************************
//���ݼ��ټ�ˢ��Pitch��Roll����
//����������ַ�������roll��pitch�����������û��#define RESTRICT_PITCH�Ͳ��õڶ��ּ��㷽��
//****************************************
void updatePitchRoll() {
	// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
	// atan2 outputs the value of -�� to �� (radians) - see http://en.wikipedia.org/wiki/Atan2
	// It is then converted from radians to degrees
	#ifdef RESTRICT_PITCH // Eq. 25 and 26
	roll = atan2(accY,accZ) * RAD_TO_DEG;
	pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
	#else // Eq. 28 and 29
	roll = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
	pitch = atan2(-accX, accZ) * RAD_TO_DEG;
	#endif
}
//****************************************
//���ݴ�����ˢ��Yaw��
//****************************************
void updateYaw() { // See: http://www.freescale.com/files/sensors/doc/app_note/AN4248.pdf
	double rollAngle,pitchAngle,Bfy,Bfx;  
	
	magX *= -1; // Invert axis - this it done here, as it should be done after the calibration
	magZ *= -1;
	
	magX *= magGain[0];
	magY *= magGain[1];
	magZ *= magGain[2];
	
	magX -= magOffset[0];
	magY -= magOffset[1];
	magZ -= magOffset[2];
	
	
	rollAngle  = kalAngleX * DEG_TO_RAD;
	pitchAngle = kalAngleY * DEG_TO_RAD;
	
	Bfy = magZ * sin(rollAngle) - magY * cos(rollAngle);
	Bfx = magX * cos(pitchAngle) + magY * sin(pitchAngle) * sin(rollAngle) + magZ * sin(pitchAngle) * cos(rollAngle);
	yaw = atan2(-Bfy, Bfx) * RAD_TO_DEG;
	
	yaw *= -1;
}













