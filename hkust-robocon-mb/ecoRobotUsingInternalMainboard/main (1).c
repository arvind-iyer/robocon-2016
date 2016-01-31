#include "main.h"
#define BUT_UP GPIO_Pin_13 //For changing the speed -GPIOC
#define BUT_UP2 GPIO_Pin_14 //For servo - GPIOC
#define BUT_UP3 GPIO_Pin_15 //For motor - GPIOC
#define LED_ON 0
#define LED_OFF 1
#define LED_RED GPIO_Pin_4 //GPIO B
#define LED_BLUE GPIO_Pin_3 //GPIO B
#define LED_GREEN GPIO_Pin_15 //GPIO A
 //Initializing the variables
    //105 max 45 min
int servo_direction = 0; //0 for 0 degrees, 1 for max_angle
int servo_angle = 1800 ;
int servo_gear = 0 ;//0: slowest 1: medium 2: high speed
int servo_increment = 10;

u16 motor_speed = 0;
u8 motor_direction = 0;
void change_servo_angle(){
        if(servo_gear == 2){
            servo_increment = 10;
            servo_gear = 0;
        }
        else if(servo_gear == 0){
            servo_increment = 45;
            servo_gear = 1;
        }
        else if(servo_gear == 1){
            servo_increment = 90;
            servo_gear = 2;
        }
}
void change_motor_speed(){
       if(motor_speed == 0){
            motor_speed = 600;
       }
       else if(motor_speed == 600){
            motor_speed = 300;
       }
       else if(motor_speed == 300){
            motor_speed = 0;
       }
}
int main()
{
    servo_init();
    u16 ticks_ms_img = 0;
    button_init();
    LED_INIT();
    ticks_init();
    while(true){
        /*if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) && servo_state == 0){ 
            servo_state = 1;
        }*/
        /*if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) && servo_speed_state == 0){ 
            GPIO_SetBits(GPIOA,GPIO_Pin_15);
            servo_speed_state = 1;
        }*/
        // 3-speed section
        if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)){
            while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));
            change_servo_angle();
            change_motor_speed();
            //servo_speed_state = 0;
        }
        if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)){
            while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));
            servo_increment = 45;
            change_servo_angle();
            change_motor_speed();
            //servo_speed_state = 0;
        }
        if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)){
            while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));
            servo_increment = 90;
            change_servo_angle();
            change_motor_speed();
            //servo_speed_state = 0;
        }
        //Controlling the servo
        if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) && (servo_direction == 0)){
           GPIO_SetBits(GPIOB,GPIO_Pin_4); //just an indicator
           while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
           while((servo_angle <= 3800)){
                servo_angle += servo_increment;
                servo_control(2, servo_angle);
                _delay_ms(10);
           }
           //servo_state = 0;
           servo_direction = 1; //change the direction
       }           
        if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) && (servo_direction == 1)){
            GPIO_SetBits(GPIOA,GPIO_Pin_15); //just an indicator
            while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
            while((servo_angle >= 1800)){
                servo_angle -= servo_increment;
                servo_control(2, servo_angle);
                _delay_ms(10);
            }
            //servo_state = 0; 
            servo_direction = 0; // change the direction
        }
        
        //Controlling the motor
       if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15) && motor_direction == 1){
           GPIO_SetBits(GPIOB,GPIO_Pin_4); //just an indicator
           motor_control(1,motor_direction,motor_speed);
           while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15));
           motor_direction = 0;
       }           
       
       if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15) && motor_direction == 0){
            GPIO_SetBits(GPIOA,GPIO_Pin_15); //just an indicator
            motor_control(1,motor_direction,motor_speed);
            while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15));
            motor_direction = 1;
       }
       if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15)){
            motor_control(1,0,1000);
       }
        
        
        //LED state
        GPIO_ResetBits(GPIOB,GPIO_Pin_4);
        GPIO_ResetBits(GPIOB,GPIO_Pin_3);
        GPIO_ResetBits(GPIOA,GPIO_Pin_15);
   }   
    return 0;
}