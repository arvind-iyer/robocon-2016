#include "can.h"
#include "can_protocol.h"
#include "lightsensor.h"

u8 enable_flag = 0;
u8 enter_calibration = 0;
u8 can_calibration_step = 0;

/* Private variables -----------------------------------------*/
CanTxMsg Tx_Queue[TX_QUEUE_LENGTH];
u8 Tx_QueueHead = 0;
u8 Tx_QueueTail = 0;
u8 Tx_QueueCounter = 0;
CanRxMsg Rx_Queue[RX_QUEUE_LENGTH];
u8 Rx_QueueHead = 0;
u8 Rx_QueueTail = 0;
u8 Rx_QueueCounter = 0;

/*****************************************************************
*				Data decomposition and reconstruction				
*		
*Author: MA Fangchang, Modifed by ZHANG Linguang
*Year: 2012			
*Description: These functions decomposite and reconstruct the s16
			  and s32 data
******************************************************************/
s32 Four_Bytes_Reconstruction(const uint8_t* buffer)
{
	s32 data=0;
	//也是低位到高位
	data |= buffer[0]; 
	data |= buffer[1] << 8;
	data |= buffer[2] << 16;
	data |= buffer[3] << 24;
	return data;
}

s16 Two_Bytes_Reconstruction(const uint8_t* buffer)
{
																																
	s16 data=0;
	data |= buffer[0]; 
	data |= buffer[1] << 8;
	return data;
}

u8 Four_Bytes_Decomposition(const s32 data, const u8 index)
{
	switch (index)
	{
		//低位到高位
		case 0: return (data & 0x000000ff);
		case 1: return (data >> 8) & 0x000000ff;
		case 2: return (data >> 16) & 0x000000ff;
		case 3: return (data >> 24) & 0x000000ff;
		default: break;
	}
	return 0;
}

u8 Two_Bytes_Decomposition(const u16 data, const u8 index)
{
	switch (index)
	{
		case 0: return (data & 0x00ff);
		case 1: return (data >> 8) & 0x00ff;
		default: break;
	}
	return 0;
}

/* General Command Encoding ------------------------------------------------------*/
CanTxMsg General_Encoding(u32 Device_ID, Data_Field Cmd_Data)
{
	u8 i;
	CanTxMsg TxMessage; 
	TxMessage.StdId = Device_ID;
	TxMessage.ExtId = 0x00;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = Cmd_Data.Data_Length;
	for(i = 0; i < Cmd_Data.Data_Length; i++)
	{
		TxMessage.Data[i] = Cmd_Data.Data[i];		
	}
	return TxMessage;
}


/* =========================================== DATA ENCODING PART ============================================== */
extern u8 white_pos;
extern u8 whiteline_position[16];
CanTxMsg Light_Sensor_Bar_Encoding(void){
	Data_Field LSB_Data;
	LSB_Data.Data_Length = 8;
    for(int i = 0; i < 8 ;i++){
        LSB_Data.Data[i] = whiteline_position[i];
    }
	//LSB_Data.Data[0] = white_pos;
	return General_Encoding(LIGHTSENSORBAR_ID, LSB_Data);		
}
//For second data partition
CanTxMsg Light_Sensor_Bar_Encoding2(void){
    Data_Field LSB_Data2;
    LSB_Data2.Data_Length = 8;
    for(int i = 0; i < 8; i++){
        LSB_Data2.Data[i] = whiteline_position[i+8];
    }
    return General_Encoding(LIGHTSENSORBAR_ID2, LSB_Data2);		

}
/* =========================================== DATA ENCODING PART ============================================== */

/**********************************************************************
*			Main Control Command Decoding --- Motor Contorl
*Description: These functions decode the cmd that the main control 
*				send to the motor control
*Variables explanation:
***********************************************************************/
/* CAN Tx message add to queue ------------------------------------------*/
void CAN_Tx_addToQueue(CanTxMsg TxMessage)
{
	Tx_Queue[Tx_QueueTail] = TxMessage;
	Tx_QueueTail++;
	if(Tx_QueueTail == TX_QUEUE_LENGTH)
		Tx_QueueTail = 0;
	Tx_QueueCounter++;
}

/* CAN Tx message dequeue and transmission -------------------------------*/
u8 CAN_Tx_dequeue(void)
{
	CanTxMsg TxMsg;
	u8 Tx_MailBox = 0;
	u16 retry = RETRY_TIMEOUT;
	if(Tx_QueueCounter != 0)
	{
		TxMsg = Tx_Queue[Tx_QueueHead];
		Tx_QueueHead++;
		if(Tx_QueueHead == TX_QUEUE_LENGTH)
			Tx_QueueHead = 0;
		Tx_QueueCounter--;
		Tx_MailBox = CAN_Transmit(CAN1, &TxMsg);							//transmit the message
		while(CAN_TransmitStatus(CAN1,Tx_MailBox) != CANTXOK && retry--);	//wait for the transmission to be finished
		return 1;
	}
	else
		return CAN_QUEUE_EMPTY;			
}


void CAN_Rx_addToQueue(CanRxMsg RxMessage)
{										   	
	Rx_Queue[Rx_QueueTail] = RxMessage;
	Rx_QueueTail++;
	if(Rx_QueueTail == RX_QUEUE_LENGTH)
		Rx_QueueTail = 0;
	Rx_QueueCounter++;
}
//for parent robot, id = 0x0C1, 0x0C2... on = 1 or 0
void Light_Sensor_Bar_Enable(u8 id, u8 on){
	u8 Tx_MailBox;
	u16 retry = RETRY_TIMEOUT;
	CanTxMsg TxMsg;
	Data_Field LSB_Data;
	LSB_Data.Data_Length = 2;
	LSB_Data.Data[0] = LSB_TOGGLE_CMD;
	LSB_Data.Data[1] = on;
	TxMsg = General_Encoding(id, LSB_Data);		
	Tx_MailBox = CAN_Transmit(CAN1, &TxMsg);							//transmit the message
	while(CAN_TransmitStatus(CAN1,Tx_MailBox) != CANTXOK && retry--){
	}
}
//for parent robot, step = 0,1,3,5,7,8, id = 0x0C1, 0x0C2...
// u8 light_sensor_cali_step[4] = {0,0,0,0}
// if (press) { call this function, light sensor step++6}
void Light_Sensor_Bar_Calibration(u8 id, u8 step){
	u8 Tx_MailBox;
	u16 retry = RETRY_TIMEOUT;
	CanTxMsg TxMsg;
	Data_Field LSB_Data;
	LSB_Data.Data_Length = 2;
	LSB_Data.Data[0] = LSB_CALIBRATE_CMD;
	LSB_Data.Data[1] = step;
	TxMsg = General_Encoding(id, LSB_Data);		
	Tx_MailBox = CAN_Transmit(CAN1, &TxMsg);							//transmit the message
	while(CAN_TransmitStatus(CAN1,Tx_MailBox) != CANTXOK && retry--){
	}	
}
/* CAN Rx message processing ---------------------------------------------*/
void CAN_Rx_Processing(CanRxMsg RxMessage){
	if(RxMessage.Data[0] == LSB_TOGGLE_CMD){
		if(RxMessage.Data[1])
			enable_flag = 1;
		else
			enable_flag = 0;
	}
	else if(RxMessage.Data[0] == LSB_CALIBRATE_CMD){
		if(RxMessage.Data[1] == 0 || RxMessage.Data[1] == 1 || RxMessage.Data[1] == 3 || RxMessage.Data[1] == 5 || RxMessage.Data[1] == 7 || RxMessage.Data[1] == 8 || RxMessage.Data[1] == 10){ 
		can_calibration_step = RxMessage.Data[1]; //0,1,3,5,7,8,10
			if(can_calibration_step == 0){
				enter_calibration = 1;
			}
		}
	}
}

/* CAN Rx message dequeue and processing ---------------------------------*/
u8 CAN_Rx_dequeue(void){
	CanRxMsg RxMsg;
	if(Rx_QueueCounter != 0)
	{
		RxMsg = Rx_Queue[Rx_QueueHead];
		Rx_QueueHead++;
		if(Rx_QueueHead == RX_QUEUE_LENGTH)
			Rx_QueueHead = 0;
		Rx_QueueCounter--;
		CAN_Rx_Processing(RxMsg);
		return 1;
	}
	else
		return CAN_QUEUE_EMPTY;			
}

void CAN_Tx_update(void){
	while(CAN_Tx_dequeue() != CAN_QUEUE_EMPTY);
}

void CAN_Rx_update(void){
	while(CAN_Rx_dequeue() != CAN_QUEUE_EMPTY);
}

/*中断在这里处理*/
void USB_LP_CAN1_RX0_IRQHandler(void){
	CanRxMsg RxMessage;
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	if(RxMessage.IDE == CAN_ID_STD)
		CAN_Rx_Processing(RxMessage);
}

u8 Enabled(void){
	return enable_flag;
}
