#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;

int main(void) {
	led_init();
	ticks_init();
	tft_easy_init();
	uart_init(COM2, 115200);
	uart_interrupt(COM2);
	buzzer_init();
	//encoder_init();
	button_init();
	gpio_init_all();
	cheap_laser_init();

//	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, 0);
//	gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, 0);
//	gpio_init(&PE8, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, 0);
//	gpio_write(&PE6, Bit_SET);
//	gpio_write(&PE7, Bit_SET);
//	gpio_write(&PE8, Bit_SET);
	
	tft_put_logo(85, 120);            
	
	while(1){
		this_loop_ticks = get_full_ticks();
		
		//Dont care if same ticks
		if (this_loop_ticks == last_loop_ticks){
			continue;
		}
		
		if ((this_loop_ticks - last_long_loop_ticks) > 300){
			for (u8 i=COM3;i<=COM5;i++){
//				uart_tx_byte(i, 0x80);
//				uart_tx_byte(i, 0x06);
//				uart_tx_byte(i, 0x02);
//				uart_tx_byte(i, 0x78);
			}
			last_long_loop_ticks = this_loop_ticks;
		}

		if ((this_loop_ticks - last_short_loop_ticks) > 10){
			led_blink(LED_D1);
			tft_clear();
			
			for (u8 i=0;i<3;i++){
				for (u8 x=0;x<7;x++){
					tft_prints(x, i+1, "%c", laser_byte_array[i][x+3]);
				}
				tft_prints(0, i+4, "%d", ticks_diff_laser[i]);
			}
			tft_update();
			last_short_loop_ticks = this_loop_ticks;
		}
		
		last_loop_ticks = this_loop_ticks;
	}
}