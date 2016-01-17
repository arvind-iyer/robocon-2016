#include "main.h"

int main(void) {
	//Init
	led_init();
	tft_easy_init();
	ticks_init();
	servo_init();
	encoder_init();
	button_init();
	track_init();
	line_sensor_init();
	
	//brings the blink on
	led_control(LED_D1, LED_ON);
    
    //state for the sensor
	u8 state = 0;
	while(1){
		button_update();
	
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}

		u8 line_sensor_state = get_line_sensor_all();
		
		tft_clear();
		tft_append_line("EL:%d ER:%d", get_count(ENCODER1), get_count(ENCODER2));
		tft_append_line("VL:%d VR:%d", get_vel(ENCODER1), get_vel(ENCODER2));
		tft_append_line("ED:%d VD:%d", get_count(ENCODER1)-get_count(ENCODER2), get_vel(ENCODER1)-get_vel(ENCODER2));
		tft_append_line("Sensor 1: %d",read_line_sensor(LINE_SENSOR_1_PIN));
        tft_append_line("Sensor 2: %d",read_line_sensor(LINE_SENSOR_2_PIN));
        
        
		//track_update();
		
		tft_update();
		
        //4 cases:
        //00
        if(read_line_sensor(LINE_SENSOR_1_PIN) == 0 && read_line_sensor(LINE_SENSOR_1_PIN == 0)){
            
        }
        //01
        if(read_line_sensor(LINE_SENSOR_1_PIN) == 0 && read_line_sensor(LINE_SENSOR_1_PIN == 1)){
            servo_control_all(2000);
        }
        //10
        if(read_line_sensor(LINE_SENSOR_1_PIN) == 1 && read_line_sensor(LINE_SENSOR_1_PIN == 0)){
            servo_control_all(4000);
        }
        //11
        if(read_line_sensor(LINE_SENSOR_1_PIN) == 1 && read_line_sensor(LINE_SENSOR_1_PIN == 1)){
            servo_control_all(3000);
        }
	}
}