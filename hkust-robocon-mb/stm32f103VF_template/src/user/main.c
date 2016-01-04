#include "main.h"
#include <stdlib.h>
#include "approx_math.h"
#define PI 3.14159265
#define MAXVEL 50


u16 ticks_img 	= (u16)-1;
u16 seconds_img = (u16)-1;

char * stringBuffer = "";
int buffPos = 0;

int main(void)
{
	stringBuffer = calloc(1, sizeof(char));
	 
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	buzzer_init();
	gyro_init();
	
	uart_init(COM2, 115200);
  uart_interrupt(COM2);

	can_init();
	can_rx_init();
	can_motor_init();
	
	s32 lastTick = get_seconds();
	
	while (1) {
		//uart_tx(COM2, "POSITION|%d|%d|%d\n", get_X(), get_Y(), get_angle());
		tft_clear();
		tft_prints(0,0,"X: %d", get_X());
		tft_prints(0,1,"Y: %d", get_Y());
		tft_prints(0,2,"Angle: %d", get_angle());
		tft_prints(0, 3, "Init: %d", gyro_available);
		tft_prints(0, 4, "En. 1: %d", get_encoder_value(MOTOR1));
		tft_prints(0, 5, "En. 2: %d", get_encoder_value(MOTOR2));
		tft_prints(0, 6, "En. 3: %d", get_encoder_value(MOTOR3));
		tft_update();
	}
}

void can_motor_set_angle(int angle)
{
	motor_set_vel(MOTOR1, int_sin(((angle)) * 10) / (float)10000 * MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR2, int_sin(((angle+120)) * 10) / (float)10000 * MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR3, int_sin(((angle+240)) * 10) / (float)10000 * MAXVEL*(-1), CLOSE_LOOP);
}

void can_motor_stop(){
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
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
        case 0: // Motor Control [direction, magnitude]
        if (contents[1] == 0) {
					can_motor_stop();
				} else {
					can_motor_set_angle(contents[0]);
				}
        break;
    }
}

void USART2_IRQHandler(void) {
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	{ // check RX interrupt
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


