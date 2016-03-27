#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stm32f10x_adc.h>

#define BLUETOOTH_MODE false

char packetQueue[30][128];
char stringBuffer[128];
int buffPos = 0;
int queuePos = 0;

uint8_t piReady = false;

uint8_t switchState = false;
uint8_t counterState = false;
uint8_t finishState = false;
uint8_t elevationCorrected = false;

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

		for (char * data = strtok(packetQueue[0], "|"); data != NULL;
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
					
		} else if (strcmp(header, "FAN_CONTROL") == 0 && contentIndex == 2){
				int fan_1_speed = contents[0];
				int fan_2_speed = contents[1];
				servo_control(SERVO2, fan_1_speed);
				//servo_control(1, fan_2_speed);
		} else if (strcmp(header, "SERVO_CONTROL") == 0 && contentIndex == 2) { // Servo Control [ID, Magnitude]
				int servoId = contents[0];
				int servoPwm = contents[1];
				servo_control(servoId, servoPwm);
    } else if (strcmp(header, "FAN_SERVO_CONTROL") == 0 && contentIndex == 2){
				int servoPwm1 = contents[0];
				int servoPwm2 = contents[1];
				servo_control(SERVO1, servoPwm1);
				TIM_SetCompare3(TIM3, servoPwm2);
		} else if (strcmp(header, "PNEUMATIC_CONTROL") == 0 && contentIndex == 2){
				int pneumaticId = contents[0];
				int pneumaticState = contents[1];
				switch (pneumaticId){
					case 0:
						pneumatic_control(GPIOE, GPIO_Pin_12, pneumaticState);
						break;
					case 1:
						pneumatic_control(GPIOE, GPIO_Pin_13, pneumaticState);
						break;
				}
		} else if (strcmp(header, "PING") == 0 && contentIndex == 1) {
			if (contents[0] == 100)
				GPIO_SetBits(GPIOE, GPIO_Pin_11);
			piReady = true;
			uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "PONG|%d|%d|%d\n",get_pos()->x, get_pos()->y, get_pos()->angle);
		}
		
		for (int i = 1; i < 30; i++) {
			for (int z = 0; z < 128; z++) {
				packetQueue[i - 1][z] = packetQueue[i][z];
			}
		}
		queuePos--;
}

void limitSwitchCheck(){
	s32 time;
	if(!gpio_read_input(&PE6) && !gpio_read_input(&PE7)){
		if(counterState == false) {
			time = get_full_ticks();
			counterState = true;
		}
		if(time - get_full_ticks() >= 20) finishState = true;
	} else if (!gpio_read_input(&PE8)) {
		if(counterState == false) {
			time = get_full_ticks();
			counterState = true;
		}
		if(time - get_full_ticks() >= 20) switchState = true;
	} else {
		counterState = false;
		switchState = false;
		finishState = false;
	}
}

int main(void) {
	tft_easy_init();
	
	ticks_init();
	buzzer_init();
	gyro_init();
	servo_init();
	pneumatic_init();
	
	// Limit switch GPIO initiaization.
	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	gpio_init(&PE8, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	
	// IR Sensor GPIO initialization.
	gpio_init(&PE9, GPIO_Speed_50MHz, GPIO_Mode_IPD, 1);
	
	// ?
	gpio_init(&PC0, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, 1);
	
	// Initialize Bluetooth/rPi Mode.
	uart_init(BLUETOOTH_MODE ? COM2 : COM1, 115200);
	uart_interrupt(BLUETOOTH_MODE ? COM2 : COM1);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastStateUpdate = get_full_ticks();
	
	while (1) {
		// Send robot state to Raspberry Pi/Bluetooth.
		uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "STATE|%d|%d|%d|%d\n", get_pos()->x, get_pos()->y, get_pos()->angle, elevationCorrected);
		
		if (queuePos > 0) {
			handleCommand();
		}
			
	
		if (get_full_ticks() - lastStateUpdate >= 10) {
			
			limitSwitchCheck();
			elevationCorrected = gpio_read_input(&PE9);

			if(finishState == true) {
				motor_set_vel(MOTOR4, 0, OPEN_LOOP);
				motor_set_vel(MOTOR5, 0, OPEN_LOOP);
				motor_set_vel(MOTOR6, 0, OPEN_LOOP);
				motor_set_vel(MOTOR7, 0, OPEN_LOOP);
			}
					
			// Display TFT to insure screen is on.
			tft_clear();
			
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
		if (byte == '\n') {
				stringBuffer[buffPos] = '\0';
			
				for (int i = 0; i < 128; i++) {
					packetQueue[queuePos][i] = stringBuffer[i];
				}
				queuePos++;

				stringBuffer[0] = '\0';
				buffPos = 0;
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

