#include "main.h"
#include <stdlib.h>
#include <stdbool.h>

#define BLUETOOTH_MODE true

char * stringBuffer = "";
int buffPos = 0;

int main(void) {
	stringBuffer = calloc(1, sizeof(char));
	tft_easy_init();
	
	ticks_init();
	buzzer_init();
	gyro_init();
	
	// Initialize Bluetooth/rPi Mode.
	uart_init(BLUETOOTH_MODE ? COM2 : COM1, 115200);
	uart_interrupt(BLUETOOTH_MODE ? COM2 : COM1);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastTick = get_seconds();
	
	while (1) {
		// Send robot state to Raspberry Pi.
		uart_tx(COM2, "STATE|%d|%d|%d", get_pos()->x, get_pos()->y, get_pos()->angle);
		
		// Display TFT to insure screen is on.
		tft_prints(0, 0, "It works!");
		tft_update();
	}
}

void handleCommand(char * command) {
    int dataIndex = 0, contentIndex = 0, header = -1;
    
    int contents[16];
    
    for (char * data = strtok(command, "|"); data != NULL; data = strtok(NULL, "|")) {
        if (dataIndex == 0) {
            header = atoi(data);
        } else {
            contents[contentIndex++] = atoi(data);
        }
        dataIndex++;
    }
    
    switch (header) {
        case 0: // Motor Control [W1, W2, W3, W4, W5, W6]
					u32 w1 = contents[0];
					u32 w2 = contents[1];
					u32 w3 = contents[2];
					u32 w4 = contents[3];
					u32 w5 = contents[4];
					u32 w6 = contents[5];
			
					// Wheel Base Motors
					motor_set_vel(MOTOR1, w1, CLOSE_LOOP);
					motor_set_vel(MOTOR2, w2, CLOSE_LOOP);
					motor_set_vel(MOTOR3, w3, CLOSE_LOOP);
				
				  // Pole Climbing Motors
					motor_set_vel(MOTOR4, w4, OPEN_LOOP);
					motor_set_vel(MOTOR5, w5, OPEN_LOOP);
					motor_set_vel(MOTOR6, w6, OPEN_LOOP);
					break;
    }
}

void handleController() {
	if (USART_GetITStatus(BLUETOOTH_MODE ? USART2 : USART1,USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(BLUETOOTH_MODE ? USART2 : USART1);
		if (byte == '\n') {
        stringBuffer[buffPos] = '\0';
        handleCommand(stringBuffer);
        buffPos = 0;
        
        memset(stringBuffer, 0, strlen(stringBuffer));
    } else {
        stringBuffer[buffPos++] = byte;
    }
		USART_ClearITPendingBit(BLUETOOTH_MODE ? USART2 : USART1, USART_IT_RXNE);
	}
}

// Bluetooth handler.
void USART2_IRQHandler(void) {
	handleController();
}

// USB handler.
void USART1_IRQHandler(void) {
	handleController();
}

