#include "main.h"



u16 ticks_img 	= (u16)-1;
u16 seconds_img = (u16)-1;

void hello(u8 id, u8 length, u8* data)
{
	buzzer_set_note_period(get_note_period(NOTE_A, 5));
	buzzer_control(1, 80);
}

int main(void)
{
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	buzzer_init();
	gyro_init();

	can_init();
	can_rx_init();
	can_motor_init();
	
	motor_set_vel(MOTOR1, 50, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 50, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 50, CLOSE_LOOP);
	
	s32 lastTick = get_seconds();
	
	
	while (1) {
				tft_clear();
				tft_prints(0,0,"X: %d", get_X());
				tft_prints(0,1,"Y: %d", get_Y());
				tft_prints(0,2,"Angle: %d", get_angle());
				tft_prints(0, 3, "Init: %d", gyro_available);
				tft_prints(0, 4, "En. 1: %d", get_encoder_value(MOTOR1));
				tft_prints(0, 5, "En. 2: %d", get_encoder_value(MOTOR2));
				tft_prints(0, 6, "En. 3: %d", get_encoder_value(MOTOR3));
				tft_update();
		
		if (get_seconds() - lastTick >= 5) {
			motor_lock(MOTOR1);
			motor_lock(MOTOR2);
			motor_lock(MOTOR3);
		}
	}
}


