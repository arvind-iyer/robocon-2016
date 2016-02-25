#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define BLUETOOTH_MODE false

char stringBuffer[512];
int buffPos = 0;

uint8_t bufferFull = false;
uint8_t piReady = false;

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
    
    if (strcmp(header, "MOTOR_CONTROL") == 0 && contentIndex == 8) { // Motor Control [W1, W2, W3, W4, W5, W6, W7, W8]
				w1 = contents[0];
				w2 = contents[1];
				w3 = contents[2];
				int w4 = contents[3];
				int w5 = contents[4];
				int w6 = contents[5];
				int w7 = contents[6];
				int w8 = contents[7];
		
				// Wheel Base Motors
				motor_set_vel(MOTOR1, w1, CLOSE_LOOP);
				motor_set_vel(MOTOR2, w2, CLOSE_LOOP);
				motor_set_vel(MOTOR3, w3, CLOSE_LOOP);
			
				// Pole Climbing Motors
				motor_set_vel(MOTOR4, w4, OPEN_LOOP);
				motor_set_vel(MOTOR5, w5, OPEN_LOOP);
				motor_set_vel(MOTOR6, w6, OPEN_LOOP);
				motor_set_vel(MOTOR7, w7, OPEN_LOOP);
			
				//Gripper Motor
					motor_set_vel(MOTOR8, w8, OPEN_LOOP);
					
			}
			if (strcmp(header, "FAN_CONTROL") == 0 && contentIndex == 2){
				int fan_1_speed = contents[0];
				int fan_2_speed = contents[1];
				servo_control(0, fan_1_speed);
				servo_control(1, fan_2_speed);
			}
			if (strcmp(header, "SERVO_CONTROL") == 0 && contentIndex == 2) { // Servo Control [ID, Magnitude]
					int servoId = contents[0];
				int servoPwm = contents[1];
				servo_control(servoId, servoPwm);
    }
		if(strcmp(header, "PNEUMATIC_CONTROL") == 0 && contentIndex == 2){
				int pneumaticId = contents[0];
				int pneumaticState = contents[1];
				switch (pneumaticId){
					case 0:
						pneumatic_control(GPIOE, GPIO_Pin_12, pneumaticState);
						break;
					//case 1:
						//pneumatic_control(pneumaticGPIO, pneumaticPin2, pneumaticState);
						//break;
				}
		}
		if (strcmp(header, "PING") == 0 && contentIndex == 1) {
			if (contents[0] == 100)
				GPIO_SetBits(GPIOE, GPIO_Pin_11);
				piReady = true;
				uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "PONG\n");
		}
		stringBuffer[0] = '\0';
		bufferFull = false;
    buffPos = 0;
}

void fan_init(){
	int start_time = get_full_ticks();
	while(get_full_ticks() - start_time <= 2000){
		if(get_full_ticks()%16 == 0){
			servo_control(SERVO1, 450);
			servo_control(SERVO2, 450);
		}
	}
}

int main(void) {
	tft_easy_init();
	
	ticks_init();
	buzzer_init();
	gyro_init();
	servo_init();
	pneumatic_init();
	gpio_init(PI_LED, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, 1);
	
	//fan_init();
	
	// Initialize Bluetooth/rPi Mode.
	uart_init(BLUETOOTH_MODE ? COM2 : COM1, 115200);
	uart_interrupt(BLUETOOTH_MODE ? COM2 : COM1);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastStateUpdate = get_full_ticks();
	
	while (1) {
		// Send robot state to Raspberry Pi/Bluetooth.
		//servo_control(SERVO1, 450);
		//servo_control(SERVO2, 450);
		//servo_control(SERVO3, 450);
		//servo_control(SERVO4, 450);
	
		if (get_full_ticks() - lastStateUpdate >= 10) {
			uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "STATE|%d|%d|%d\n", get_pos()->x, get_pos()->y, get_pos()->angle);
			
			if (bufferFull == true) {
				handleCommand();
			}
			
			
			
			tft_clear();
			
//		if(get_full_ticks()%10000 > 0 && get_full_ticks()%1000<=5000){
//			tft_prints(0,4,"ON");
//			pneumatic_control(GPIOE, GPIO_Pin_12,1);
//			pneumatic_control(GPIOE, GPIO_Pin_13,1);
//			pneumatic_control(GPIOE, GPIO_Pin_14,1);
//			pneumatic_control(GPIOE, GPIO_Pin_15,1);
//			pneumatic_control(GPIOD, GPIO_Pin_8,1);
//			pneumatic_control(GPIOD, GPIO_Pin_9,1);
//			pneumatic_control(GPIOD, GPIO_Pin_10,1);
//			pneumatic_control(GPIOD, GPIO_Pin_11,1);
//			pneumatic_control(GPIOB, GPIO_Pin_9,1);

//		}
//		if(get_full_ticks()%10000 > 5000 && get_full_ticks()%1000<=9999){
//			tft_prints(0,4,"OFF");
//			pneumatic_control(GPIOE, GPIO_Pin_12,0);
//			pneumatic_control(GPIOE, GPIO_Pin_13,0);
//			pneumatic_control(GPIOE, GPIO_Pin_14,0);
//			pneumatic_control(GPIOE, GPIO_Pin_15,0);
//			pneumatic_control(GPIOD, GPIO_Pin_8,0);
//			pneumatic_control(GPIOD, GPIO_Pin_9,0);
//			pneumatic_control(GPIOD, GPIO_Pin_10,0);
//			pneumatic_control(GPIOD, GPIO_Pin_11,0);
//			pneumatic_control(GPIOB, GPIO_Pin_9,0);

//		}
			
			// Display TFT to insure screen is on.
			tft_prints(0, 0, "X: %d", get_pos()->x);
			tft_prints(0, 1, "Y: %d", get_pos()->y);
			tft_prints(0, 2, "A: %d", get_pos()->angle);
			tft_prints(0, 3, "Time: %d", get_full_ticks());
			if(piReady) {
				tft_prints(0 , 4, "Pi Ready");
			}
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

