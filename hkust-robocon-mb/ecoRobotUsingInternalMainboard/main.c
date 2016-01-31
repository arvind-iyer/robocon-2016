#include "main.h"
#define BUT_UP GPIO_Pin_13 //For changing the speed -GPIOC
#define BUT_UP2 GPIO_Pin_14 //For servo - GPIOC
#define BUT_UP3 GPIO_Pin_15 //For motor - GPIOC
#define LED_ON 0
#define LED_OFF 1
#define LED_RED GPIO_Pin_4 //GPIO B
#define LED_BLUE GPIO_Pin_3 //GPIO B
#define LED_GREEN GPIO_Pin_15 //GPIO 
u16 motor_speed = 0;
u8 motor_direction = 0;

int main()
{
    u16 ticks_ms_img = 0;
    button_init();
    LED_INIT();
    ticks_init();
    tft_init(0,BLACK,WHITE,RED);
    servo_init();
    u8 state = 0; //0 or 1 or 2
    while(true){
        if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)){
            GPIO_SetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_ResetBits(GPIOA,GPIO_Pin_15);
            servo_control(2000);
        }
        if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)){
            GPIO_ResetBits(GPIOB,GPIO_Pin_4);
            GPIO_SetBits(GPIOB,GPIO_Pin_3);
            GPIO_ResetBits(GPIOA,GPIO_Pin_15);
            servo_control(3000);

        }
        if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)){
            GPIO_ResetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_SetBits(GPIOA,GPIO_Pin_15);
            servo_control(4000);
        }
        
        u8 line_sensor_state = get_line_sensor_all();
		switch(line_sensor_state){
			case track_turn_happy:
				servo_control(SERVO_MED);
               GPIO_ResetBits(GPIOB,GPIO_Pin_4);
            GPIO_SetBits(GPIOB,GPIO_Pin_3);
            GPIO_ResetBits(GPIOA,GPIO_Pin_15);
                state = 0 ;
				break;
			case track_turn_left_smooth:
                state = 1;
                   GPIO_SetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_ResetBits(GPIOA,GPIO_Pin_15);
				servo_control(SERVO_MAX);
				break;
			case track_turn_left_90:
				servo_control(SERVO_MAX);
               GPIO_SetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_ResetBits(GPIOA,GPIO_Pin_15);
                state = 1;
				break;
			case track_turn_right_smooth:
				state = 2;
               GPIO_ResetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_SetBits(GPIOA,GPIO_Pin_15);
				servo_control(SERVO_MIN);
				break;
			case track_turn_right_90:
				servo_control(SERVO_MIN);
               GPIO_ResetBits(GPIOB,GPIO_Pin_4);
            GPIO_ResetBits(GPIOB,GPIO_Pin_3);
            GPIO_SetBits(GPIOA,GPIO_Pin_15);
                state = 2;
				break;
            case track_turn_leave:
                switch(state){
                    case 0:
                        servo_control(SERVO_MED);
                        break;
                    case 1:
                        servo_control(SERVO_MAX);
                        break;
                    case 2:
                        servo_control(SERVO_MIN);
                        break;
                }
                break;
			default:
                GPIO_SetBits(GPIOB,GPIO_Pin_4);
                GPIO_SetBits(GPIOB,GPIO_Pin_3);
                GPIO_SetBits(GPIOA,GPIO_Pin_15);
				break;
		}
        
        tft_clear();
        tft_prints(0,0,"Hello");
		tft_prints(0,1,BYTETOBINARYPATTERN" %d", BYTETOBINARY(line_sensor_state), line_sensor_state);
        tft_prints(0,2,"State: %d",state);
		//track_update();
		
		tft_update();
        
        
        
   }   
    return 0;
}