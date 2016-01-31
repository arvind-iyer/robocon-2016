#include "main.h"

int main(void)
{
    SystemInit();
    led_init();
    
    //IMPORTANT: TM_DELAY_INIT MUST BE BEFORE ticks_init!!!
    TM_DELAY_Init();
    TM_DELAY_EnableDelayTimer();
    ticks_init();
    TM_SERVO_t Servo1, Servo2; //Create 2 Servo Objects
    
    //Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
    TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
    TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
    
    uint16_t ticks_ms_img = 0;
    Delayms(2000);
  /* Infinite loop */
  while (1)
  {
      if(get_ms_ticks() % 1000 == 0){
        TM_SERVO_SetDegrees(&Servo1, 30);
        LED_ON(LED_1);
        LED_ON(LED_2);
        LED_ON(LED_3);
        LED_ON(LED_4);
        LED_ON(LED_5);
      }
      if(get_ms_ticks() % 1000 == 500){
        TM_SERVO_SetDegrees(&Servo1, 90);
        LED_OFF(LED_1);
        LED_OFF(LED_2);
        LED_OFF(LED_3);
        LED_OFF(LED_4);
        LED_OFF(LED_5);
          
      }
 }
    
    return 0;
}






