#include "laser_range_finder.h"
#include "lcd_main.h"

static char cmd[65];
volatile static LRF_stat status;
volatile static char feedback[65];


LRF_stat LRF_init(){
	/*initiation for the uart*/
	uart_init(LRF_COMx,19200);
	uart_interrupt(LRF_COMx);
	
	/*change to SCIP2.0 Protocol*/
	stringcpy(cmd,"SCIP2.0\n");
	if( LRF_send_cmd() == ECHO_NOT_MATCH)	return ECHO_NOT_MATCH;
	status = WAIT_FOR_STATUS;
	while(status);
	if (feedback[0]!='0')	return SCIP2_NOT_SUCESS;
	stringcpy(feedback,"");
	
	/*start the laser measurement*/
	stringcpy(cmd,"BM\n");
	if(LRF_send_cmd() == ECHO_NOT_MATCH)	return ECHO_NOT_MATCH;
	status = WAIT_FOR_STATUS;
	while(status);
	if ( feedback[2] != ((feedback[0]+feedback[1])&63)+0x30) return SUM_NOT_EQUAL;
	switch (feedback[1]){
		case '0':
			break;
		case '1':
			return LASER_MALFUNCTION;
		case '2':
			break;
	}
	stringcpy(feedback,"");
	return NORMAL;
}

LRF_stat LRF_send_cmd(){
	status = WAIT_FOR_ECHO;
	stringcpy(feedback,"");
	uart_tx(LRF_COMx,cmd);
	while(status);
	if(!stringcmp(cmd,feedback))
		return ECHO_NOT_MATCH;
	stringcpy(feedback,"");
	return NORMAL;
}

LRF_Handler{
	if(USART_GetITStatus(COM_USART[LRF_COMx],USART_IT_RXNE) != RESET)
	{ // check RX interrupt
		char data[2] = {USART_ReceiveData(COM_USART[LRF_COMx]),'\0'};
		tft_clear();
		tft_prints(0,0,"feedback: %s",feedback);
		tft_update();
		
		switch(status){
			
			case WAIT_FOR_ECHO:
				if(data[0] == '\n')
					status = NORMAL;
				else
					stringcat(feedback,data);
				break;
				
			case WAIT_FOR_STATUS:
				if(data[0] == '\n')
					status = NORMAL;
				else
					stringcat(feedback,data);
				break;
				
			}
				

		USART_ClearITPendingBit(COM_USART[LRF_COMx], USART_IT_RXNE);
	}
}

