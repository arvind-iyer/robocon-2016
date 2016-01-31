#include "main.h"
#include <stdlib.h>

char * stringBuffer = "";
int buffPos = 0;

int main(void) {
	stringBuffer = calloc(1, sizeof(char));
	 
	tft_init(2, RED, WHITE, BLUE);
	ticks_init();
	buzzer_init();
	gyro_init();
	led_init();
	
	uart_init(COM2, 115200);
  uart_interrupt(COM2);

	can_init();
	can_rx_init();
	can_motor_init();

	setTargetPos(-1000, 1000, 5);

	while (1) {
		if (get_seconds()>3){
			uint32_t this_ticks = get_ticks();
			if (this_ticks % 50 == 0){
				this_ticks%100 ? led_control(LED_D1, LED_ON) : led_control(LED_D1, LED_OFF);
				if (isRunning()){
					this_ticks%100 ? led_control(LED_D2, LED_ON) : led_control(LED_D2, LED_OFF);
					chase();				
				}
			}
		}
	}
}

/*
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
        case 0: // Motor Control [direction, magnitude]
        if (contents[1] == 0) {
					lockAllMotors();
				} else {
					setRobotVelocity(contents[0] * 10 - get_angle(), contents[1]);
				}
				break;
				case 1: // Testing
					if (ROBOT_MOVING == 0) {
						setTargetLocation(contents[0], contents[1]);
					} else {
						lockAllMotors();
						ROBOT_MOVING = 0;
					}
        break;
    }
}

// Bluetooth handler.
void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2,USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(USART2);
		if (byte == '\n') {
        stringBuffer[buffPos] = '\0';
        handleCommand(stringBuffer);
        buffPos = 0;
        
        memset(stringBuffer, 0, strlen(stringBuffer));
    } else {
        stringBuffer[buffPos++] = byte;
    }
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
*/

