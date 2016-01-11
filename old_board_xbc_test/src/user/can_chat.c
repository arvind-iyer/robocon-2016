#include "can_chat.h"

	
char received_string[128];
u8 receive_string_pointer = 0;

void ReceiveHandler(CanRxMsg msg){
	u8 i = 0, j = 0;
	led_control(LED_D2_Pin, 1);
	
	while(i<8){
		if (msg.Data[i]==endOfMessage){
			uart_tx(COM1, "OtherGuy: ");
			tft_clear();
			for (j=0;j<receive_string_pointer;j++){
				tft_prints(j%15, j/15 , "%c", received_string[j]);
				uart_tx_byte(COM1, received_string[j]);
			}
			uart_tx_byte(COM1, '\n');
			tft_update();
			receive_string_pointer = 0;
			break;
		}else{
			received_string[receive_string_pointer++] = msg.Data[i];
		}
		i++;
	}
	led_control(LED_D2_Pin, 0);
}

void can_chat_init(u32 filterBase){
	can_init();
	can_rx_init();
	can_rx_add_filter(filterBase, CAN_RX_MASK_DIGIT_0_F, ReceiveHandler);
}


void SendChatMessage(u32 target_id, u8 vel[], u8 length){
	CAN_MESSAGE msg;
	u8 count = 0;
	u8 i = 0;
	led_control(LED_D2_Pin, 1);
	
	uart_tx(COM1, "Yourself: ");
	
	//Send message 8 in a pack
	while(length>=8){
		CAN_MESSAGE msg;
	
		msg.id = target_id;
		msg.length = 8;
		for (i=0;i<8;i++){
			msg.data[i] = (u8) vel[count++];
			uart_tx_byte(COM1, msg.data[i]);
		}
		
		can_tx_enqueue(msg);
		length -= 8;
	}
	
	//Send the remaining message and end of message

	msg.id = target_id;
	msg.length = length+1;
	for (i=0;i<length;i++){
		msg.data[i] = (u8) vel[count++];
		uart_tx_byte(COM1, msg.data[i]);
	}
	msg.data[length] = (u8) endOfMessage;
	uart_tx_byte(COM1, '\n');
		
	can_tx_enqueue(msg);
	led_control(LED_D2_Pin, 0);
}

