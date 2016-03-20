#define uchar unsigned char
#define uint unsigned int

#define	CE	    PEout(3)
#define	CSN		PEout(2)
#define	SCK	    PEout(5)
#define	MOSI	PEout(4)
#define	MISO	PCout(1)
#define	IRQ		PEout(6)



//uchar TxBuf[20]={"1234567890abcdefghij"};
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 32 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 32 uints TX payload
uchar  TX_ADDRESS[TX_ADR_WIDTH]= {0xE7,0xE7,0xE7,0xE7,0xE7};	//本地地址
uchar  RX_ADDRESS[RX_ADR_WIDTH]= {0xE7,0xE7,0xE7,0xE7,0xE7};	//接收地址
///***************************************NRF24L01寄存器指令*******************************************************
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
///*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define NRFRegSTATUS    0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道1接收数据长度
#define RX_PW_P2        0x13  // 接收频道2接收数据长度
#define RX_PW_P3        0x14  // 接收频道3接收数据长度
#define RX_PW_P4        0x15  // 接收频道4接收数据长度
#define RX_PW_P5        0x16  // 接收频道5接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
///*****************************子函数集*********************************************************
uchar NRF24SPI_Send_Byte(uchar dat);
uchar SPI_WR_Reg(uchar reg, uchar value);
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Len);
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Len);
uchar nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);
uchar SPI_RD_Reg(uchar reg);
void init_NRF24L01(void);
void TX_Mode(void);
void RX_Mode(void);
void NRF_Send(void);
uchar NRF24L01_Check(void);
//----------------------------------------------
//发送一字节[MOSI和MISO数据传递]
//----------------------------------------------
uchar NRF24SPI_Send_Byte(uchar dat)
{
  	uchar i;
	for (i = 0; i < 8; i++) //output 8-bit
	{
		//写入1位数据
		MOSI=(dat & 0x80);	//output 'uchar', MSB to MOSI
		dat<<= 1;           // shift next bit into MSB..
		
		//读取1位数据
		SCK = 1;                   	// Set SCK high..
		if (MISO){
		 	dat|= 1;
		}else{             			// capture current MISO bit
		 	dat &= 0xFE;
		}
		SCK = 0;                    // ..then set SCK low again
	}
	return(dat);                	// return read uchar
}
//----------------------------------------------
//延时n个10us
//----------------------------------------------
void Delay_n10us(uint n)	
{
   for(;n>0;n--)
   {
   	 unsigned char a,b;
     for(b=1;b>0;b--)
         for(a=2;a>0;a--);
   }
}
//----------------------------------------------
//NRF24L01检测是否存在
//----------------------------------------------
uchar NRF24L01_Check(void)
{
	uchar bu[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uchar bu1[5];
	uchar i;   	 
	SPI_Write_Buf(WRITE_REG+TX_ADDR,bu,5);	  //写入5个字节的地址.	
	SPI_Read_Buf(TX_ADDR,bu1,5);              //读出写入的地址  	
	for(i=0;i<5;i++)if(bu1[i]!=0XA5)break;					   
	if(i!=5)return 1;                               //NRF24L01不在位	
	return 0;		                                //NRF24L01在位
}	
//----------------------------------------------
//NRF24L01初始化
//---------------------------------------------- 	 
void init_NRF24L01(void)
{
	uchar buf[5]={0};
	Delay_n10us(10);
	CE = 0;    		// chip enable
	CSN= 0;    		// Spi disable 
	
	SPI_Read_Buf(TX_ADDR, buf, TX_ADR_WIDTH);//debug 测试原来的本地地址：复位值是：0xE7 0xE7 0xE7 0xE7 0xE7
  
//	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
//	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址

//	
//	SPI_WR_Reg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
//	SPI_WR_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
//	SPI_WR_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 设置自动重发时间和次数：500us + 86us, 10 retrans...
//	SPI_WR_Reg(WRITE_REG + RF_CH, 22);        //   设置信道工作为2.4GHZ，收发必须一致
//	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
//	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB
//  
//  SPI_RD_Reg(WRITE_REG + EN_AA);
//  SPI_RD_Reg(WRITE_REG + EN_RXADDR);
//  SPI_RD_Reg(WRITE_REG + RF_CH);
//  SPI_RD_Reg(WRITE_REG + RX_PW_P0);
//  SPI_RD_Reg(WRITE_REG + RF_SETUP);
}
//----------------------------------------------
//函数：uchar SPI_Read(uchar reg)
//功能：NRF24L01的SPI时序,读取指定寄存器的内容
//---------------------------------------------- 
uchar SPI_RD_Reg(uchar reg)
{
	uchar reg_val;
	CSN = 0;                			// CSN low, initialize SPI communication...
	NRF24SPI_Send_Byte(reg);            // Select register to read from..
	reg_val = NRF24SPI_Send_Byte(0);    // ..then read registervalue
	CSN = 1;                			// CSN high, terminate SPI communication	
	return(reg_val);        			// return register value
}
//----------------------------------------------
//函数：uchar SPI_Write(uchar reg)
//功能：NRF24L01的SPI时序,写入指定寄存器的内容
//---------------------------------------------- 
uchar SPI_WR_Reg(uchar reg, uchar value)
{
	uchar status;
	CSN = 0;                   			// CSN low, init SPI transaction
	status = NRF24SPI_Send_Byte(reg);	// select register
	NRF24SPI_Send_Byte(value);          // ..and write value to it..
	CSN = 1;                   			// CSN high again
	return(status);            			// return nRF24L01 status uchar
}
//---------------------------------------------- 
//函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Len)
//功能: 用于读数据
//参数：reg：为寄存器地址
//		pBuf：为待读出数据地址
//		uchars：读出数据的个数
//---------------------------------------------- 
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Len)
{
	uint status,i;
	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = NRF24SPI_Send_Byte(reg);   // Select register to write to and read status uchar
	for(i=0;i<Len;i++){
	 	pBuf[i] = NRF24SPI_Send_Byte(0);
	}
	CSN = 1;                          
	return(status);                    	// return nRF24L01 status uchar
}
//----------------------------------------------
//函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Len)
//功能: 用于写数据
//参数：reg:为寄存器地址
//		pBuf：为待写入数据地址
//		uchars：写入数据的个数
//----------------------------------------------
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Len)
{
	uint status,i;
	CSN = 0;            				//SPI使能       
	status = NRF24SPI_Send_Byte(reg);   
	for(i=0; i<Len; i++){
	   NRF24SPI_Send_Byte(*pBuf);
		 pBuf ++;
	}
	CSN = 1;           					//关闭SPI
	return(status);   
}
//----------------------------------------------
//函数：void SetRX_Mode(void)
//功能：数据接收配置 
//----------------------------------------------
void RX_Mode(void)
{
    uchar buf[5]={0};
	CE = 0;
	SPI_Read_Buf(TX_ADDR,buf, TX_ADR_WIDTH);	//debug 测试原来的本地地址：复位值是：0xE7 0xE7 0xE7 0xE7 0xE7
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x03);//SPI_WR_Reg(WRITE_REG + CONFIG, 0x0f); // IRQ收发完成中断响应，16位CRC	，主接收
	//SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	// 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 	// 写接收端地址
	SPI_WR_Reg(WRITE_REG + EN_AA, 0);		      		//频道0自动	ACK应答允许	
	SPI_WR_Reg(WRITE_REG + EN_RXADDR,0x01);  			//允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_WR_Reg(WRITE_REG + SETUP_RETR,0x1a); 			//设置自动重发时间和次数：500us + 86us, 10 retrans...
	SPI_WR_Reg(WRITE_REG + RF_CH, 40);        			//设置信道工作为2.4GHZ，收发必须一致
	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 	//设置接收数据长度，本次设置为32字节
	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x0F);   			//设置发射速率为2MHZ，发射功率为最大值0dB
	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0F);
	CE = 1; 
	Delay_n10us(20); //200us
}
//----------------------------------------------
//函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//功能：数据读取后放入rx_buf接收缓冲区中
//----------------------------------------------
uchar nRF24L01_RxPacket(unsigned char* rx_buf)
{
    uchar flag=0;
    uchar status;
 	
	status=SPI_RD_Reg(NRFRegSTATUS);	//读取状态寄存其来判断数据接收状况
	
	if(status & 0x40)					//判断是否接收到数据
	{
	  	 CE=0; 			//SPI使能
		 SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);	// read receive payload from RX_FIFO buffer
		 flag =1;											//读取数据完成标志
	}
	SPI_WR_Reg(WRITE_REG+NRFRegSTATUS, status);   			//接收到数据后RX_DR,TX_DS,MAX_RT都置高为1，通过写1来清楚中断标志
	return flag;
}
//----------------------------------------------
//函数：void TX_Mode(void)
//功能：数据接收配置 
//----------------------------------------------
void TX_Mode(void)
{
    CE = 0;
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x02);   //0x0E		 // IRQ收发完成中断响应，16位CRC，主发送
	
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_WR_Reg(WRITE_REG + EN_AA, 0);//0x01);      					//  频道0自动	ACK应答允许	
	SPI_WR_Reg(WRITE_REG + EN_RXADDR,0);// 0x01);  					//  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_WR_Reg(WRITE_REG + SETUP_RETR, 0);//0x1a); 					// 设置自动重发时间和次数：500us + 86us, 10 retrans...
	SPI_WR_Reg(WRITE_REG + RF_CH, 40);        						//   设置信道工作为2.4GHZ，收发必须一致
	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 				//设置接收数据长度，本次设置为32字节
	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x0F);   						//设置发射速率为2MHZ，发射功率为最大值0dB

	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0E);
    CE = 1;
}
//----------------------------------------------
//函数：void nRF24L01_TxPacket(unsigned char * tx_buf)
//功能：发送tx_buf中数据
//----------------------------------------------
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	CE = 0;			//StandBy I模式	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	CE = 1;		 //置高CE，激发数据发送
}
///************************************主函数************************************************************
//void NRF_Send(void)
//{
//  //static uint counter=0;
//  static uchar flag=0;  
//   
//   //TX_Mode();
//   
//   //IntervalTimems(100);
//   if(flag==0)
//   {
//      flag=1;
//      //memcpy(TxBuf, "1234567890abcdefghij", 20);
//      nRF24L01_TxPacket(TxBuf);
//   }
//   else
//   {
//      flag=0;
//      //memcpy(TxBuf, "abcdefghij1234567890", 20);
//      nRF24L01_TxPacket(TxBuf);
//   }
//   
//}
#endif   /*__NRF24L01_H__ */