#include "main.h"
#include "math.h"

int main(void)
{
	tft_init(1, BLACK, RED, GREEN);
	can_init();
	can_rx_init();
	led_init();
	ticks_init();
	can_motor_init();
	
	while(1){
		u32 clock_ticks = get_ticks();
		if (clock_ticks%150==0){
			led_control(LED_D1, clock_ticks % 300);
			tft_clear();
			tft_prints(0,0,"%d", clock_ticks);
			tft_update();
		}
		motor_set_vel(MOTOR1, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR2, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR3, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR4, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR5, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR6, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR7, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR8, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR9, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR10, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR11, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR12, 5, CLOSE_LOOP);
		motor_set_vel(MOTOR13, 5, CLOSE_LOOP);
	}
}
