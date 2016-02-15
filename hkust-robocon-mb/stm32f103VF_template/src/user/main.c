#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define BLUETOOTH_MODE true

char stringBuffer[512];
int buffPos = 0;

uint8_t bufferFull = false;

int w1 = 0, w2 = 0, w3 = 0;

int custom_atoi(char *s)
{
	 int minus = 0;
	 if (*s == '-') minus = 1, s++;

	 int res = 0;

	 while (isdigit(*s))
	 {
			res = res*10 + (*s++ - '0');
	 }

	 return minus ? -res : res;
}

void handleCommand() {
    int dataIndex = 0, contentIndex = 0;
		char * header;

		int contents[16];

		for (char * data = strtok(stringBuffer, "|"); data != NULL;
				data = strtok(NULL, "|")) {
			if (dataIndex == 0) {
				header = data;
			} else {
				contents[contentIndex++] = custom_atoi(data);
			}
			dataIndex++;
		}
    
    if (strcmp(header, "MOTOR_CONTROL") == 0 && contentIndex == 6) { // Motor Control [W1, W2, W3, W4, W5, W6]
				w1 = contents[0];
				w2 = contents[1];
				w3 = contents[2];
				int w4 = contents[3];
				int w5 = contents[4];
				int w6 = contents[5];
		
				// Wheel Base Motors
				motor_set_vel(MOTOR1, w1, CLOSE_LOOP);
				motor_set_vel(MOTOR2, w2, CLOSE_LOOP);
				motor_set_vel(MOTOR3, w3, CLOSE_LOOP);
			
				// Pole Climbing Motors
				motor_set_vel(MOTOR4, w4, OPEN_LOOP);
				motor_set_vel(MOTOR5, w5, OPEN_LOOP);
				motor_set_vel(MOTOR6, w6, OPEN_LOOP);
		} else if (strcmp(header, "SERVO_CONTROL") == 0 && contentIndex == 2) { // Servo Control [ID, Magnitude]
				int servoId = contents[0];
				int servoPwm = contents[1];
				servo_control(servoId, servoPwm);
    }
		
		stringBuffer[0] = '\0';
		bufferFull = false;
    buffPos = 0;
}

int main(void) {
	tft_easy_init();
	
	ticks_init();
	buzzer_init();
	gyro_init();
	servo_init();
	
	// Initialize Bluetooth/rPi Mode.
	uart_init(BLUETOOTH_MODE ? COM2 : COM1, 115200);
	uart_interrupt(BLUETOOTH_MODE ? COM2 : COM1);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastStateUpdate = get_full_ticks();
	
	while (1) {
		// Send robot state to Raspberry Pi/Bluetooth.
		if (get_full_ticks() - lastStateUpdate >= 10) {
			uart_tx(COM2, "STATE|%d|%d|%d\n", get_pos()->x, get_pos()->y, get_pos()->angle);
			
			if (bufferFull == true) {
				handleCommand();
			}
			
			// Display TFT to insure screen is on.
			tft_prints(0, 0, "It works!");	
			tft_prints(0, 1, "X: %d", get_pos()->x);
			tft_prints(0, 2, "Y: %d", get_pos()->y);
			tft_prints(0, 3, "A: %d", get_pos()->angle);
			tft_prints(0, 4, "Time: %d", get_full_ticks());
			tft_update();
			
			lastStateUpdate = get_full_ticks();
		}
	}
}

void handleController() {
	if (USART_GetITStatus(BLUETOOTH_MODE ? USART2 : USART1, USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(BLUETOOTH_MODE ? USART2 : USART1);
		if (bufferFull == false) {
			if (byte == '\n') {
					stringBuffer[buffPos] = '\0';
					bufferFull = true;
			} else {
				//if (isalnum(byte) || byte == '|') {
					stringBuffer[buffPos++] = byte;
				//}
			}
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

