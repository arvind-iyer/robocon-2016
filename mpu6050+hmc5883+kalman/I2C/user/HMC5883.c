#define   uchar unsigned char
#define   uint unsigned int	

//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
#define	HMC5883_Addr   0x3C	//磁场传感器器件地址

unsigned char BUF[8];                         //接收数据缓存区			   	
extern uchar	SlaveAddress;   			//IIC写入时的地址字节数据，+1为读取

extern int magX, magY, magZ;	//hmc最原始数据
extern uchar SlaveAddress;   	//IIC写入时的地址字节数据，+1为读取
extern uchar Single_ReadI2C(uchar REG_Address);						//读取I2C数据
extern void  Single_WriteI2C(uchar REG_Address,uchar REG_data);		//向I2C写入数据
//**************************************
//初始化HMC5883，根据需要请参考pdf进行修改
//**************************************
void InitHMC5883()
{
	SlaveAddress=HMC5883_Addr;
    Single_WriteI2C(0x02,0x00);  //
	Single_WriteI2C(0x01,0xE0);  //
}
//**************************************
//从HMC5883连续读取6个数据放在BUF中
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

//	if(magX>0x7fff)magX-=0xffff;//补码表示滴~所以要转化一下	  
//	if(magY>0x7fff)magY-=0xffff;	
// 	if(magZ>0x7fff)magZ-=0xffff;
}
