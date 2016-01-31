
#include "main.h"
#include <stdlib.h>

char * stringBuffer = "";
int buffPos = 0;

int main(void) {
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
	/*if (ROBOT_MOVING == 1) {
			pursueTarget();
		}
		
		tft_clear();
		tft_prints(0, 0, "X: %d", get_pos()->x);
		tft_prints(0, 1, "Y: %d", get_pos()->y);
		tft_prints(0, 2, "Angle: %d", get_pos_raw()->angle);
		tft_prints(0, 3, "E1: %d", get_encoder_value(MOTOR1));
		tft_prints(0, 4, "E2: %d", get_encoder_value(MOTOR2));
		tft_prints(0, 5, "E3: %d", get_encoder_value(MOTOR3));
		tft_prints(0, 6, "VX: %d", velXPid.output);
		tft_prints(0, 7, "VY: %d", velYPid.output);
		tft_prints(0, 8, "VW: %d", velWPid.output);
		tft_prints(0, 9, "Time: %u", (unsigned int) (get_full_ticks() - TARGET_TICKS) / (uint16_t) 1000);
		tft_update();*/
	}
}

void handleCommand(char * command) {
    int dataIndex = 0, contentIndex = 0, header = -1;
    
    int32_t contents[16];
    
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
					s32 angle = (contents[0] * 10);
					s32 vel = contents[1];
					setRobotVelocity(vel, angle, 0, CLOSE_LOOP);
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
				case 2: // Shift Gyro [+x, +y, -x, -y]
					if (contents[0] == 1) plus_x();
					else if (contents[1] == 1) plus_y();
					else if (contents[2] == 1) minus_x();
					else if (contents[3] == 1) minus_y();
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

