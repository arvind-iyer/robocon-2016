#include "main.h"

u8 y_index;

int main(void) {
	tft_init(2, RED, WHITE, BLUE);
	ticks_init();
	buzzer_init();
	gyro_init();
	led_init();
	
	uart_init(COM2, 115200);
  uart_interrupt(COM2);

	can_init();
	can_rx_init();
	can_motor_init();

	while (1) {
		y_index = 0;
		tft_clear();
		tft_prints(0, y_index++, "X:  %d", get_X());
		tft_prints(0, y_index++, "Y:  %d", get_Y());
		tft_prints(0, y_index++, "A:  %d", get_angle());
		
		tft_prints(0, y_index++, "E1: %d", get_encoder_value(0));
		tft_prints(0, y_index++, "E2: %d", get_encoder_value(1));
		tft_prints(0, y_index++, "E3: %d", get_encoder_value(2));
		tft_update();
	}
}


