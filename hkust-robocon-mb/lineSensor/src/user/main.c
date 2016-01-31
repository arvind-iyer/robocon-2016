#include "main.h"
#define SERVO_LEFT 2400
#define SERVO_RIGHT 700

int main(void) {
	//Init
	led_init();
	//tft_easy_init();
    tft_init(1,BLACK,WHITE,RED);
	ticks_init();
	servo_init();
	encoder_init();
	button_init();
	track_init();
	line_sensor_init();
	
	//brings the blink on
	led_control(LED_D1, LED_ON);
    
    //Servo State after turning 90 degrees:
    u8 state = 0; //0 or 1 or 2
	
	while(1){
		button_update();
		
		//track_test();
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}

		u8 line_sensor_state = get_line_sensor_all();
		
		
		
		switch(line_sensor_state){
			case track_turn_happy:
				servo_control_all(SERVO_MED);
                state = 0 ;
				break;
			case track_turn_left_smooth:
                state = 1;
				servo_control_all(SERVO_LEFT);
				break;
			case track_turn_left_90:
				servo_control_all(SERVO_LEFT);
                state = 1;
				break;
			case track_turn_right_smooth:
				state = 2;
				servo_control_all(SERVO_RIGHT);
				break;
			case track_turn_right_90:
				servo_control_all(SERVO_RIGHT);
                state = 2;
				break;
            case track_turn_leave:
                switch(state){
                    case 0:
                        servo_control_all(SERVO_MED);
                        break;
                    case 1:
                        servo_control_all(SERVO_LEFT);
                        break;
                    case 2:
                        servo_control_all(SERVO_RIGHT);
                        break;
                }
                break;
			default:
				led_control(LED_D3, LED_ON);
				break;
		}
        
        tft_clear();
		tft_append_line("EL:%d ER:%d", get_count(ENCODER1), get_count(ENCODER2));
		tft_append_line("VL:%d VR:%d", get_vel(ENCODER1), get_vel(ENCODER2));
		tft_append_line("ED:%d VD:%d", get_count(ENCODER1)-get_count(ENCODER2), get_vel(ENCODER1)-get_vel(ENCODER2));
		tft_append_line(BYTETOBINARYPATTERN" %d", BYTETOBINARY(line_sensor_state), line_sensor_state);
        tft_append_line("90 State: %d",state);
		//track_update();
		
		tft_update();
	}
    return 0;
}