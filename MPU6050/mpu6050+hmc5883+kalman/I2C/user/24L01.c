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
uchar  TX_ADDRESS[TX_ADR_WIDTH]= {0xE7,0xE7,0xE7,0xE7,0xE7};	//���ص�ַ
uchar  RX_ADDRESS[RX_ADR_WIDTH]= {0xE7,0xE7,0xE7,0xE7,0xE7};	//���յ�ַ
///***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
///*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define NRFRegSTATUS    0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
///*****************************�Ӻ�����*********************************************************
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
//����һ�ֽ�[MOSI��MISO���ݴ���]
//----------------------------------------------
uchar NRF24SPI_Send_Byte(uchar dat)
{
  	uchar i;
	for (i = 0; i < 8; i++) //output 8-bit
	{
		//д��1λ����
		MOSI=(dat & 0x80);	//output 'uchar', MSB to MOSI
		dat<<= 1;           // shift next bit into MSB..
		
		//��ȡ1λ����
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
//��ʱn��10us
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
//NRF24L01����Ƿ����
//----------------------------------------------
uchar NRF24L01_Check(void)
{
	uchar bu[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uchar bu1[5];
	uchar i;   	 
	SPI_Write_Buf(WRITE_REG+TX_ADDR,bu,5);	  //д��5���ֽڵĵ�ַ.	
	SPI_Read_Buf(TX_ADDR,bu1,5);              //����д��ĵ�ַ  	
	for(i=0;i<5;i++)if(bu1[i]!=0XA5)break;					   
	if(i!=5)return 1;                               //NRF24L01����λ	
	return 0;		                                //NRF24L01��λ
}	
//----------------------------------------------
//NRF24L01��ʼ��
//---------------------------------------------- 	 
void init_NRF24L01(void)
{
	uchar buf[5]={0};
	Delay_n10us(10);
	CE = 0;    		// chip enable
	CSN= 0;    		// Spi disable 
	
	SPI_Read_Buf(TX_ADDR, buf, TX_ADR_WIDTH);//debug ����ԭ���ı��ص�ַ����λֵ�ǣ�0xE7 0xE7 0xE7 0xE7 0xE7
  
//	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
//	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ

//	
//	SPI_WR_Reg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������	
//	SPI_WR_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
//	SPI_WR_Reg(WRITE_REG + SETUP_RETR, 0x1a); // �����Զ��ط�ʱ��ʹ�����500us + 86us, 10 retrans...
//	SPI_WR_Reg(WRITE_REG + RF_CH, 22);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
//	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
//	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
//  
//  SPI_RD_Reg(WRITE_REG + EN_AA);
//  SPI_RD_Reg(WRITE_REG + EN_RXADDR);
//  SPI_RD_Reg(WRITE_REG + RF_CH);
//  SPI_RD_Reg(WRITE_REG + RX_PW_P0);
//  SPI_RD_Reg(WRITE_REG + RF_SETUP);
}
//----------------------------------------------
//������uchar SPI_Read(uchar reg)
//���ܣ�NRF24L01��SPIʱ��,��ȡָ���Ĵ���������
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
//������uchar SPI_Write(uchar reg)
//���ܣ�NRF24L01��SPIʱ��,д��ָ���Ĵ���������
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
//������uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Len)
//����: ���ڶ�����
//������reg��Ϊ�Ĵ�����ַ
//		pBuf��Ϊ���������ݵ�ַ
//		uchars���������ݵĸ���
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
//������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Len)
//����: ����д����
//������reg:Ϊ�Ĵ�����ַ
//		pBuf��Ϊ��д�����ݵ�ַ
//		uchars��д�����ݵĸ���
//----------------------------------------------
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Len)
{
	uint status,i;
	CSN = 0;            				//SPIʹ��       
	status = NRF24SPI_Send_Byte(reg);   
	for(i=0; i<Len; i++){
	   NRF24SPI_Send_Byte(*pBuf);
		 pBuf ++;
	}
	CSN = 1;           					//�ر�SPI
	return(status);   
}
//----------------------------------------------
//������void SetRX_Mode(void)
//���ܣ����ݽ������� 
//----------------------------------------------
void RX_Mode(void)
{
    uchar buf[5]={0};
	CE = 0;
	SPI_Read_Buf(TX_ADDR,buf, TX_ADR_WIDTH);	//debug ����ԭ���ı��ص�ַ����λֵ�ǣ�0xE7 0xE7 0xE7 0xE7 0xE7
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x03);//SPI_WR_Reg(WRITE_REG + CONFIG, 0x0f); // IRQ�շ�����ж���Ӧ��16λCRC	��������
	//SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	// д���ص�ַ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 	// д���ն˵�ַ
	SPI_WR_Reg(WRITE_REG + EN_AA, 0);		      		//Ƶ��0�Զ�	ACKӦ������	
	SPI_WR_Reg(WRITE_REG + EN_RXADDR,0x01);  			//������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_WR_Reg(WRITE_REG + SETUP_RETR,0x1a); 			//�����Զ��ط�ʱ��ʹ�����500us + 86us, 10 retrans...
	SPI_WR_Reg(WRITE_REG + RF_CH, 40);        			//�����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 	//���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x0F);   			//���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB
	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0F);
	CE = 1; 
	Delay_n10us(20); //200us
}
//----------------------------------------------
//������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//���ܣ����ݶ�ȡ�����rx_buf���ջ�������
//----------------------------------------------
uchar nRF24L01_RxPacket(unsigned char* rx_buf)
{
    uchar flag=0;
    uchar status;
 	
	status=SPI_RD_Reg(NRFRegSTATUS);	//��ȡ״̬�Ĵ������ж����ݽ���״��
	
	if(status & 0x40)					//�ж��Ƿ���յ�����
	{
	  	 CE=0; 			//SPIʹ��
		 SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);	// read receive payload from RX_FIFO buffer
		 flag =1;											//��ȡ������ɱ�־
	}
	SPI_WR_Reg(WRITE_REG+NRFRegSTATUS, status);   			//���յ����ݺ�RX_DR,TX_DS,MAX_RT���ø�Ϊ1��ͨ��д1������жϱ�־
	return flag;
}
//----------------------------------------------
//������void TX_Mode(void)
//���ܣ����ݽ������� 
//----------------------------------------------
void TX_Mode(void)
{
    CE = 0;
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x02);   //0x0E		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_WR_Reg(WRITE_REG + EN_AA, 0);//0x01);      					//  Ƶ��0�Զ�	ACKӦ������	
	SPI_WR_Reg(WRITE_REG + EN_RXADDR,0);// 0x01);  					//  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_WR_Reg(WRITE_REG + SETUP_RETR, 0);//0x1a); 					// �����Զ��ط�ʱ��ʹ�����500us + 86us, 10 retrans...
	SPI_WR_Reg(WRITE_REG + RF_CH, 40);        						//   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_WR_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 				//���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x0F);   						//���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB

	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0E);
    CE = 1;
}
//----------------------------------------------
//������void nRF24L01_TxPacket(unsigned char * tx_buf)
//���ܣ�����tx_buf������
//----------------------------------------------
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	CE = 0;			//StandBy Iģʽ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
	//SPI_WR_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	CE = 1;		 //�ø�CE���������ݷ���
}
///************************************������************************************************************
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