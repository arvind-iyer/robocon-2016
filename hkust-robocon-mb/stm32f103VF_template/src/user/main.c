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

uint8_t counterState = false;
uint8_t elevationCorrected = false;
uint8_t prevFinishState = false;
bool DEBUG_STATE = false;
bool armSwitchUp = false;
bool armSwitchDown = false;
bool moveState = false;
bool allowUpdate = false;

int w1 = 0, w2 = 0, w3 = 0, ls_value = 0;

s32 time=0, moveTime = 0;

void servo_adc_init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//-------------TimeBase Initialization-----------//
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			// counter will count up (from 0 to FFFF)
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;					//timer clock = dead-time and sampling clock 	
 	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
 
 	//------------------------------//
 	TIM_TimeBaseStructure.TIM_Prescaler = 143;												//clk=72M/(71+1)= Hz, interval=?
  TIM_TimeBaseStructure.TIM_Period = 10000;												//pulse cycle= 20000
  //------------------------------//
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
	// ------------OC Init Configuration------------//
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
 	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
 	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
 	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the output
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; // this part disable the Nstate
 	//------------------------------//
  TIM_OCInitStructure.TIM_Pulse = 6667;														// this part sets the initial CCR value
  //------------------------------//
  
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

void adc_servo_control(int id, u16 val){
	switch (id){
		case 0:
			TIM_SetCompare1(TIM3, val);
			break;
		case 1:
			TIM_SetCompare2(TIM3, val);
			break;
	}
}

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
					motor_set_vel(MOTOR8, w8, CLOSE_LOOP);
		} else if (strcmp(header, "FREE_MOTORS") == 0 && contentIndex == 0) {
					motor_set_vel(MOTOR1, 0, OPEN_LOOP);
					motor_set_vel(MOTOR2, 0, OPEN_LOOP);
					motor_set_vel(MOTOR3, 0, OPEN_LOOP);
		} else if (strcmp(header, "FAN_CONTROL") == 0 && contentIndex == 2){
				int fan_1_speed = contents[0];
				int fan_2_speed = contents[1];
				servo_control(SERVO2, fan_1_speed);
				adc_servo_control(0, fan_2_speed);
				//servo_control(1, fan_2_speed);
		} else if (strcmp(header, "SERVO_CONTROL") == 0 && contentIndex == 2) { // Servo Control [ID, Magnitude]
				int servoId = contents[0];
				int servoPwm = contents[1];
				servo_control(servoId, servoPwm);
    } else if (strcmp(header, "FAN_SERVO_CONTROL") == 0 && contentIndex == 2){
				int servoPwm1 = contents[0];
				int servoPwm2 = contents[1];
				servo_control(SERVO1, servoPwm1);
				adc_servo_control(1, servoPwm2);
		} else if (strcmp(header, "PNEUMATIC_CONTROL") == 0 && contentIndex == 2){
				DEBUG_STATE = true;
				int pneumaticId = contents[0];
				int pneumaticState = contents[1];
				switch (pneumaticId){
					case 0:
						pneumatic_control(GPIOE, GPIO_Pin_12, pneumaticState);
						break;
					case 1:
						pneumatic_control(GPIOE, GPIO_Pin_13, pneumaticState);
						break;
					case 2:
						pneumatic_control(GPIOE, GPIO_Pin_14, pneumaticState);
					break;
				}
		} else if (strcmp(header, "PING") == 0 && contentIndex == 2) {
			piReady = true;
			gyro_set_shift_x(contents[0]);
			gyro_set_shift_y(contents[1]);
			allowUpdate = true;
			uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "PONG|%d|%d|%d\n",get_pos()->x, get_pos()->y, get_pos()->angle);
		}
		
		for (int i = 1; i < 30; i++) {
			for (int z = 0; z < 128; z++) {
				packetQueue[i - 1][z] = packetQueue[i][z];
			}
		}
		queuePos--;
}

//void limitSwitchCheck(){
//	if(gpio_read_input(&PE6) || gpio_read_input(&PE7)){
//		if(counterState == false) {
//			time = get_full_ticks();
//			counterState = true;
//		}
//		if(get_full_ticks() - time >= 20) finishState = true;
//	} else if (gpio_read_input(&PE9)) {
//		if(counterState == false) {
//			time = get_full_ticks();
//			counterState = true;
//		}
//		if(get_full_ticks() - time >= 20) switchState = true;
//	} else {
//		counterState = false;
//		switchState = false;
//		finishState = false;
//	}
//}

bool armDir = false, fixingArm = false, climbLimit = false; // False = down, True = up
long lastLimitCheck = 0;
bool b1=0, b2=0, b3=0, b4=0, prevb4=0;
void limitSwitchCheck() {
  b1 = gpio_read_input(&PE6);
	b2 = gpio_read_input(&PE7);
	b3 = gpio_read_input(&PE9);
	b4 = gpio_read_input(&PE11);
	if (get_full_ticks() - lastLimitCheck >= 20 && (fixingArm)) {
		fixingArm = false;
		climbLimit  = false;
		tft_prints(5,5, "STOP");
		motor_set_vel (MOTOR8, 0, CLOSE_LOOP);
		lastLimitCheck = get_full_ticks();
	}
	
	if (b1 || b2) {
		armDir = b2 ? 1 : 0;
		motor_set_vel(MOTOR8, armDir ? -10 : 10, CLOSE_LOOP);
		fixingArm = true;
		lastLimitCheck = get_full_ticks();
		// If fixing arm, always make sure IR never gets triggered until fixingArm is false.
	}
	else if(b3) {
		motor_set_vel(MOTOR4, 0, OPEN_LOOP);
		motor_set_vel(MOTOR5, 0, OPEN_LOOP);
		motor_set_vel(MOTOR6, 0, OPEN_LOOP);
		motor_set_vel(MOTOR7, 0, OPEN_LOOP);
		//climbLimit = true;	
		//lastLimitCheck = get_full_ticks();
	}
	if(prevb4 != b4){
		prevb4 = b4;
		uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "SWITCH|4|%d\n", b4);
	}
}

int main(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// Limit switch GPIO initialization.
	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_IPD, 1);
	gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_IPD, 1);
	gpio_init(&PE9, GPIO_Speed_50MHz, GPIO_Mode_IPD, 1);
	gpio_init(&PE11, GPIO_Speed_50MHz,GPIO_Mode_IPD, 1);
	// IR Sensor GPIO initialization.
	gpio_init(&PE8, GPIO_Speed_50MHz, GPIO_Mode_IPU, 1);
	
	// ADC Servo initialization.
	gpio_init(&PA6, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, 1);
	//gpio_init(&PA7, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, 1);
	
	// ?
	gpio_init(&PC0, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, 1);
	
	tft_easy_init();
	ticks_init();
	buzzer_init();
	gyro_init();
	servo_init();
	//servo_adc_init();
	pneumatic_init();
	ls_init();
	
	// Initialize Bluetooth/rPi Mode.
	uart_init(BLUETOOTH_MODE ? COM2 : COM1, 115200);
	uart_interrupt(BLUETOOTH_MODE ? COM2 : COM1);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastStateUpdate = get_full_ticks();
	
	while (1) {
		// Send robot state to Raspberry Pi/Bluetooth.
		uart_tx(BLUETOOTH_MODE ? COM2 : COM1, "STATE|%d|%d|%d|%d|%d\n", get_pos()->x, get_pos()->y, get_pos()->angle, elevationCorrected, ls_value);
		
		if (queuePos > 0) {
			handleCommand();
		}
			
	
		if (get_full_ticks() - lastStateUpdate >= 10) {
			
			if(allowUpdate){
				limitSwitchCheck();
				if(!fixingArm) {elevationCorrected = gpio_read_input(&PE8);}
			}
			pneumatic_control(GPIOE, GPIO_Pin_15, 1);
			
			ls_value = get_ls_reading();
					
			// Display TFT to insure screen is on.
			tft_clear();
			
			tft_prints(0, 0, "X: %d", get_pos()->x);
			tft_prints(0, 1, "Y: %d", get_pos()->y);
			tft_prints(0, 2, "A: %d", get_pos()->angle);
			tft_prints(0, 3, "Time: %d", get_full_ticks());
			if(piReady) {
				tft_prints(0 , 4, "Pi Ready");
			}
			tft_prints(0,5, "%d", ls_value);
			tft_prints(0, 6, "L %d | %d | %d", gpio_read_input(&PE6), gpio_read_input(&PE7), gpio_read_input(&PE11));
			//if(DEBUG_STATE) {
				//tft_prints(0,5,"TRIGGERED");
				//DEBUG_STATE = false;
			//}
		
			
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

