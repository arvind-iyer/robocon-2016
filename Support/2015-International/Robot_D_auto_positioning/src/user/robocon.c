#include "robocon.h"

/*
Important note on changes needed in Robot C and Robot D

Wheel ID should be changed in wheel_base.h




Details are written in the file
*/




//system values
static u16 ticks_img 	= (u16)-1;
static u32 last_sent_OS_time = 0;
static bool key_trigger_enable = true;
static bool servo_released = false;
static bool use_xbc_input = false;



void wheel_base_joystick_control(void)
{
	//wheel_base_set_vel(0,0,0);
	if (button_pressed(BUTTON_JS2_LEFT))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(-wheel_base_get_joystick_speed(),0,0);
		//commandReceivedFlag=1;
	}
	if (button_pressed(BUTTON_JS2_RIGHT))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(wheel_base_get_joystick_speed(),0,0);
	}
	if (button_pressed(BUTTON_JS2_UP))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(0,wheel_base_get_joystick_speed(),0);
	}
	if (button_pressed(BUTTON_JS2_DOWN))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(0,-wheel_base_get_joystick_speed(),0);
	}
	if (button_pressed(BUTTON_JS1_RIGHT))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(0,0,wheel_base_get_joystick_speed());
	}
	if (button_pressed(BUTTON_JS1_LEFT))
	{
		wheel_base_joyStickCommandFlag_on();
		wheel_base_set_vel(0,0,-wheel_base_get_joystick_speed());
	}
	if (button_pressed(BUTTON_JS1_CENTER)==1)
	{
		wheel_base_decrease_joystick_speed();
	}
	if (button_pressed(BUTTON_JS2_CENTER)==1)
	{
		wheel_base_increase_joystick_speed();
	}
}

void set_xbc_input_allowed(bool set)
{
	use_xbc_input = set;
}




static void handle_bluetooth_input(void)
{
	if (key_trigger_enable && !bluetooth_is_key_release())
	{
		last_sent_OS_time = get_full_ticks();
		key_trigger_enable = false;
		//wheel_base_pid_off();
		switch (wheel_base_bluetooth_get_last_char())
		{
			case 'k':
				racket_hit();
			break;
			case 'l':
				if(ROBOT == 'D')
				racket_calibrate();
			break;
			case 'o':
				if(ROBOT == 'D')
				motor_lock(RACKET);
			break;
			case 'y'://The kewl LASER SERVE
				if(ROBOT == 'D'){
				//is_laser_serve_enabled(1);
				toggle_servo();
				}
				else
				{
					//Do pneu serve
					//current_pneumatic = !current_pneumatic;
					//racket_pneumatic_set(current_pneumatic);
				}
				break;
			case 'u'://Normal Serve
				if(ROBOT == 'D'){
				//is_laser_serve_enabled(0);
				racket_start_serve();
				}
				
				break;
			case 'b':
				set_xbc_input_allowed(true);
				break;
			case 'v':
				set_xbc_input_allowed(false);
				break;
			
			case 'p':
				if(ROBOT == 'D')
				racket_stop();
				key_trigger_enable = true;
			break;
			case '=':
				//For both robots
				racket_change_hit_vel(2);
				key_trigger_enable = true;
				break;
			case '-':
				racket_change_hit_vel(-2);
				key_trigger_enable = true;
				break;
			//Increase and decreaase laser hit delay
			case '_':
				if(ROBOT == 'D'){
					racket_change_laser_hit_delay(-1);
					key_trigger_enable = true;
				}
				else
				{
					
				}
				break;
			case '+':
				racket_change_laser_hit_delay(+1);
				key_trigger_enable = true;
				break;
				
			case '.':
				racket_change_serve_delay(+5);
				break;
			case ',':
				racket_change_serve_delay(-5);
				break;
			case '[':		//only temporary
				plus_x();
				key_trigger_enable = true;
				break;
			case ']':		// only temporary
				minus_x();
				key_trigger_enable = true;
				break;
			case '{':		//only temporary
				plus_y();
				key_trigger_enable = true;
				break;
			case '}':		// only temporary
				minus_y();
				key_trigger_enable = true;
				break;
			
			//Increase Speed Mode
			case 'P':
				wheel_base_set_speed_mode(wheel_base_get_speed_mode() + 1);
				break;
			//Decrease Speed Mode
			case 'O':
				wheel_base_set_speed_mode(wheel_base_get_speed_mode() - 1);
				break;
			
			case 'f':
			case 'g':
				wheel_base_pid_off();
	 }
	}
	else if (bluetooth_is_key_release())
	{
		key_trigger_enable = true;
	}
	
}

void robocon_main(void)
{
	motor_set_acceleration(RACKET, 3000);
//  static char last_key;
  // Send the acceleration data
	wheel_base_tx_acc();
	//racket_init();
	racket_stop();
	gpio_init(&PE9, GPIO_Speed_10MHz, GPIO_Mode_Out_PP, 1);		// pneu matic GPIO
	gpio_init(&PE5, GPIO_Speed_10MHz, GPIO_Mode_Out_PP, 1);		//laser sensor GPIO OUT
	gpio_init(&PE6, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, 1);		// laser sensor GPIO OUT 2
	gpio_init(&PE8, GPIO_Speed_10MHz, GPIO_Mode_Out_PP, 1);		//pneumatic GPIO 2
	//gpio_init(&);
	//gpio_init(&PE7, GPIO_Speed_50MHz, GPIO_Mode_IPU, 0);		// laser sensor GPIO IN
	gpio_write(&PE5, 0);		//write 1 to Laser sensor
	gpio_write(&PE6, 0);		//write 1 to Laser sensor 2
	//register_special_char_function('m',print);
	while (1) {
		if (ticks_img != get_ticks()) {
			ticks_img = get_ticks();
			
			if (ticks_img % 10 == 0) {
        wheel_base_update();	//wheel_base_update now also handles auto positioning system
				bluetooth_update();
        handle_bluetooth_input();
 
        button_update();
				// Every 10 ms (100 Hz)
        
			}
      racket_update();
			if (ticks_img % 250 == 1) {
				// Every 250 ms (4 Hz)
				battery_adc_update();
			}
			
			
			if (get_seconds() % 10 == 2 && ticks_img == 2) {
				// Every 10 seconds (0.1 Hz)
				battery_regular_check();
			}

      if (ticks_img % 100 == 3) {
				if(special_char_handler_bt_get_last_char() == 'k')
				{
					tft_prints(0,7, "HIT");
					tft_update();
				}
      }
			
			if (ticks_img % 100 == 3) {
				// Every 100 ms (10 Hz)
				wheel_base_tx_position();
			}
			
			if (ticks_img % 500 == 4) {
				led_control(LED_D3, (LED_STATE) (ticks_img == 0));
			}
			
			if (ticks_img % 50 == 5) {
				wheel_base_joystick_control();
				if (button_pressed(BUTTON_1) > 10 || button_pressed(BUTTON_2) > 10) {
					/** Stop the wheel base before return **/
					return; 
				}
			}
			
			if (ticks_img % 50 == 7) {
				// Every 50 ms (20 Hz)
				/** Warning: try not to do many things after tft_update(), as it takes time **/

				WHEEL_BASE_VEL vel = wheel_base_get_vel();
				tft_clear();
				draw_top_bar();

				tft_prints(0, 1, "V:(%3d,%3d,%3d)", vel.x, vel.y, vel.w);
				tft_prints(0, 2, "Speed: %d", wheel_base_get_speed_mode());				char s[3] = {wheel_base_bluetooth_get_last_char(), '\0'};
        if (s[0] == '[' || s[0] == ']') {
          // Replace "[" and "]" as "\[" and "\]"
          s[1] = s[0];
          s[0] = '\\';
        }
				
        tft_prints(0, 6, "Char: %s (%d) %c", s, wheel_base_bluetooth_get_last_char(), special_char_handler_bt_get_last_char());
				tft_prints(0,3,"SHIT: (%d, %d)", gyro_get_shift_x(), gyro_get_shift_y());
				tft_prints(0,4,"Serve_delay: %d",racket_get_serve_delay());
				tft_prints(0,5, "Switch = %d", gpio_read_input(&PE3));
				//tft_prints(0,2, "x%d y%d", gyro_get_shift_x(), gyro_get_shift_y());
				tft_prints(0,7, "LASER%d %d", gpio_read_input(LASER_GPIO),racket_get_laser_hit_delay);
				tft_prints(0,8,"Encoder: %d", get_encoder_value(RACKET));
				//tft_prints(0,7,"init: %d", get_init_enc());
				tft_prints(0,9,"Racket: %d", racket_get_vel());
				//tft_prints(0,3,"stop enc = %d",racket_get_last_stop_encoder_value());
				tft_update();
			}
			
		}
		
	}	
}