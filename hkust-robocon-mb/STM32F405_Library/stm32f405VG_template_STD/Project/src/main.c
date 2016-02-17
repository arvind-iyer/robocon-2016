#include "main.h"
#include <string.h>


//Create servo objects
TM_SERVO_t Servo1, Servo2;


//Sensor bar data
u16 data1[8];
u16 data2[8];
u16 sensor_output[16];

void receive(CanRxMsg msg)
{
	for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
    
}

void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}

void fill_array(){
    for(int i = 0; i < 8; i++){
        sensor_output[i] = data2[7-i];
    }
    for(int i = 0; i < 8; i++){
        sensor_output[8+i] = data1[7-i];
    }
}

void print_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        
        tft_prints(i,1,"%d",sensor_output[i]);
    }


}

//Command Handler for Raspberry Pi Controller
void handleCommand(char * command) {
	int dataIndex = 0, contentIndex = 0, header = -1;

	int contents[16];

	for (char * data = strtok(command, "|"); data != NULL; data = strtok(NULL, "|")) {
		if (dataIndex == 0) {
			header = atoi(data);
		} else {
			contents[contentIndex++] = atoi(data);
		}
		dataIndex++;
	}

	switch (header) {
	case 0: // bla
		TM_SERVO_SetDegrees(&Servo1, 30);
		LED_ON(LED_1);
		LED_ON(LED_2);
		LED_ON(LED_3);
		LED_ON(LED_4);
		LED_ON(LED_5);
		break;
	case 1:
		TM_SERVO_SetDegrees(&Servo1, 90);
		LED_OFF(LED_1);
		LED_OFF(LED_2);
		LED_OFF(LED_3);
		LED_OFF(LED_4);
		LED_OFF(LED_5);
		break;
	}
}

void cantest_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_ResetBits(GPIOA,GPIO_Pin_11); //Rx
    GPIO_ResetBits(GPIOA,GPIO_Pin_12); //Tx

}

void uarttest_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
}


int main(void) {

	led_init();

	//IMPORTANT: TM_DELAY_INIT MUST BE BEFORE ticks_init!!!
	TM_DELAY_Init();
	

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);

	//Initialize the 2 USART Ports
    char buffer[512];
    
    //TM_USART_Init(USART3, TM_USART_PinsPack_1
    //LCD Initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED); //Init LCD
    
    //Initialize Gyro module
    gyro_init();

	//Initialize I2C Modules (For future Gyro using I2C protocol, not USART anymore) PB10 & PB12
    //TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000); 
	//cantest_init();
    //uarttest_init();
    
    //Initialize the CAN protocol
    can_init();
    can_rx_init();
    can_motor_init();
    motor_set_vel(MOTOR1,10,OPEN_LOOP);
    motor_set_vel(MOTOR2,10,OPEN_LOOP);
    motor_set_vel(MOTOR3,10,OPEN_LOOP);
    motor_set_vel(MOTOR4,250,OPEN_LOOP);
    motor_set_vel(MOTOR5,250,OPEN_LOOP);
    motor_set_vel(MOTOR6,250,OPEN_LOOP);

    
    //Initialize encoder
    u32 ticks_ms_img = 0;
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();
            //testing();
            tft_clear();
             
            tft_prints(0,0,"Count: %d",get_ticks());
            tft_prints(0,1,"Motor1: %d",get_encoder_value(MOTOR1));
            tft_prints(0,2,"Motor2: %d",get_encoder_value(MOTOR2));
            tft_prints(0,3,"Motor3: %d",get_encoder_value(MOTOR3));
            tft_prints(0,4,"Motor4: %d",get_encoder_value(MOTOR4));
            tft_prints(0,5,"Motor5: %d",get_encoder_value(MOTOR5));
            tft_prints(0,6,"Motor6: %d",get_encoder_value(MOTOR6));
            
            tft_update();
        }    
	}

	return 0;
}