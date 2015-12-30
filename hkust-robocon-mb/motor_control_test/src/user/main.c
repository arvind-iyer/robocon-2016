#include "main.h"
#include "math.h"

int main(void)
{
	tft_init(1, RED, BLACK, BLUE);
	can_init();
	can_rx_init();
	led_init();
	ticks_init();
	can_motor_init();
	
	while(1){
		u32 clock_ticks = get_ticks();
		if (clock_ticks%150==0){
			led_control(LED_D1, clock_ticks % 300);
		}
		motor_set_vel(MOTOR1, 50, CLOSE_LOOP);
		motor_set_vel(MOTOR2, 50, CLOSE_LOOP);
		motor_set_vel(MOTOR3, 50, CLOSE_LOOP);
	}
}
