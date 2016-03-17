#define   uchar unsigned char
#define   uint unsigned int	

//����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
#define	HMC5883_Addr   0x3C	//�ų�������������ַ

unsigned char BUF[8];                         //�������ݻ�����			   	
extern uchar	SlaveAddress;   			//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ

extern int magX, magY, magZ;	//hmc��ԭʼ����
extern uchar SlaveAddress;   	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
extern uchar Single_ReadI2C(uchar REG_Address);						//��ȡI2C����
extern void  Single_WriteI2C(uchar REG_Address,uchar REG_data);		//��I2Cд������
//**************************************
//��ʼ��HMC5883��������Ҫ��ο�pdf�����޸�
//**************************************
void InitHMC5883()
{
	SlaveAddress=HMC5883_Addr;
    Single_WriteI2C(0x02,0x00);  //
	Single_WriteI2C(0x01,0xE0);  //
}
//**************************************
//��HMC5883������ȡ6�����ݷ���BUF��
//**************************************
void updateHMC5883()
{
	SlaveAddress=HMC5883_Addr;
	Single_WriteI2C(0x00,0x14); 
	Single_WriteI2C(0x02,0x00); 
//	Delayms(10);
	
	BUF[1]=Single_ReadI2C(0x03);//OUT_X_L_A
	BUF[2]=Single_ReadI2C(0x04);//OUT_X_H_A
	BUF[3]=Single_ReadI2C(0x07);//OUT_Y_L_A
	BUF[4]=Single_ReadI2C(0x08);//OUT_Y_H_A
	BUF[5]=Single_ReadI2C(0x05);//OUT_Z_L_A
	BUF[6]=Single_ReadI2C(0x06);//OUT_Y_H_A
	
	magX=(BUF[1] << 8) | BUF[2]; //Combine MSB and LSB of X Data output register
	magY=(BUF[3] << 8) | BUF[4]; //Combine MSB and LSB of Y Data output register
	magZ=(BUF[5] << 8) | BUF[6]; //Combine MSB and LSB of Z Data output register

//	if(magX>0x7fff)magX-=0xffff;//�����ʾ��~����Ҫת��һ��	  
//	if(magY>0x7fff)magY-=0xffff;	
// 	if(magZ>0x7fff)magZ-=0xffff;
}
