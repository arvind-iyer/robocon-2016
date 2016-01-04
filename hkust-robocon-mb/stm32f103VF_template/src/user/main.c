#include "main.h"
#include "math.h"
#define PI 3.14159265
#define MAXVEL 100

u16 ticks_last = 0;
u8 state = 0;
u8 string_pointer = 0;
u8 input_string[128];

void can_motor_set_angle(int angle)
{
	motor_set_vel(MOTOR1, sin((angle)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR2, sin((angle+120)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR3, sin((angle+240)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
}

void can_motor_stop(){
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

int main(void)
{
	led_init();
	tft_init(0, BLUE, WHITE, BLACK);
	ticks_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	while (1) {
	}
}
