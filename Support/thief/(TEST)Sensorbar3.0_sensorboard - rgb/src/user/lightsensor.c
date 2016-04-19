#include "lightsensor.h"

#define DEBUG_MODE

u8 current_colour = RED;
u8 Detect_val[16]= {0};
extern volatile u16 ADC_val[16];
volatile u32 Avg_ADC_val[3][16] = {0};
//u32 *Avg_ADC_val_pt = Avg_ADC_val;

Colour_Target RED_target[16];
Colour_Target *RED_pt = RED_target;
Colour_Target BLUE_target[16];
Colour_Target *BLUE_pt = RED_target;
Colour_Target YELLOW_target[16];
Colour_Target *YELLOW_pt = RED_target;
Colour_Target WHITE_target[16];
Colour_Target *WHITE_pt = RED_target;

volatile Colour_Target Current_colour[16];
u16 Background_val[16];
CanTxMsg TxMessage_prev;

u8 whiteline_position[16];
u8 white_pos = 88;

extern u8 can_calibration_step;
u16 Difference_2Colour(Colour_Target x, Colour_Target y){
	return (u16)Sqrt((s32)((x.R-y.R)*(x.R-y.R) + (x.G-y.G)*(x.G-y.G) + (x.B-y.B)*(x.B-y.B)));
}

u8 resultColour(u8 id, Colour_Target current){
	u8 i = 0;
	u16 Distance_R = Difference_2Colour(current,RED_target[id]);
	u16 Distance_B = Difference_2Colour(current,BLUE_target[id]);
	u16 Distance_Y = Difference_2Colour(current,YELLOW_target[id]);
	u16 Distance_W = Difference_2Colour(current,WHITE_target[id]);
	u16 min = Distance_W;

	if(min > Distance_B)
		min = Distance_B;
	if(min > Distance_Y)
		min = Distance_Y;
	if(min > Distance_R)
		min = Distance_R;
	
	if(min == Distance_R)
		return RED;
	if(min == Distance_B)
		return BLUE;
	if(min == Distance_Y)
		return YELLOW;
	if(min == Distance_W)
		return WHITE;
}

void CaptureData(void){
	u8 i = 0; u8 j = 0;
	for (i = 0; i<16; ++i){
		Avg_ADC_val[current_colour][i] = 0;
	}
	for (i = 0; i < SAMPLETIMES ; i++){
		for(j = 0; j<16 ; ++j){
			Avg_ADC_val[current_colour][j] += (u32)ADC_val[j] ;
		}
	}
	for(j = 0; j<16; j++){
		Avg_ADC_val[current_colour][j] /= SAMPLETIMES;
	}
}

void ShowWhiteLinePos(void){
	u16 position = 0;
	u8 i = 0;
	u8 counter = 0;
	for(i=0; i<16; i++){
		if(whiteline_position[i]){
			position += i*2;
			counter++;
		}
	}
	position /= counter;
	if(counter){
		IndicatorControl((u8)position);
		white_pos = (u8)position;
	}
	else{
		IndicatorControl(100);
		white_pos = 88;
	}
	#ifdef DEBUG_MODE
		printf("White Line Position: %d \n\r",(u8)position);
	#endif
}

void AnalysisData(void){
	u8 id_array[16] = {10,11,12,13,0,1,2,3,4,5,6,7,14,15,8,9};
	u8 i = 0;
	for(i=0;i<16;i++){
		Current_colour[i].R = Avg_ADC_val[RED][id_array[i]];
		Current_colour[i].G = Avg_ADC_val[GREEN][id_array[i]];
		Current_colour[i].B = Avg_ADC_val[BLUE][id_array[i]];
		whiteline_position[i] = 0;
	}
	for(i=0;i<16;i++){
		if(resultColour(i, Current_colour[i]) == WHITE){
				whiteline_position[i] = 1;
		}
	}
	#ifdef DEBUG_MODE
		for(i=4;i<8;i++){
			switch(resultColour(i, Current_colour[i])){
				case(RED):
					printf("RED");
					break;
				case(YELLOW):
					printf("YELLOW");
					break;
				case(BLUE):
					printf("BLUE");
					break;
				case(WHITE):
					printf("WHITE");
					break;
			}
			printf("	%d   %d   %d \n\r",Current_colour[i].R, Current_colour[i].G, Current_colour[i].B);
		}
		printf("=\n\r");
	#endif
	ShowWhiteLinePos();
}

void SwitchLight(void){
	current_colour++;
	if(current_colour == 3){
		current_colour = 0;
		LED_Control(0,0,0);
		AnalysisData();
	}
	switch(current_colour){
		case(RED):
			LED_Control(0,1,0);
			break;
		case(GREEN):
			LED_Control(1,0,0);
			break;
		case(BLUE):
			LED_Control(0,0,1);
			break;
		default:
			LED_Control(0,0,0);
			break;
	}
}

void ColourTarget_Init(void){
	u8 i = 0;
	for(i=0;i<16;i++){		
		RED_target[i].R = 1446;
		RED_target[i].G = 1152;
		RED_target[i].B = 1368;
		
		BLUE_target[i].R = 700;
		BLUE_target[i].G = 973;
		BLUE_target[i].B = 1200;
		
		YELLOW_target[i].R = 1407;
		YELLOW_target[i].G = 1806;
		YELLOW_target[i].B = 881;
		
		WHITE_target[i].R = 1200;
		WHITE_target[i].G = 1200;
		WHITE_target[i].B = 1200;
		
		Current_colour[i].R = 0;
		Current_colour[i].G = 0;
		Current_colour[i].B = 0;
	}
}
u8 last_white_pos = 0;
//void DataTransmit(void){
//	u8 Tx_MailBox;
//	u16 retry = RETRY_TIMEOUT;
//	CanTxMsg TxMsg;
//	//if(last_white_pos != white_pos){
//		//u8 tx_flag=0;
//		//for(tx_flag=0;tx_flag<3;tx_flag++)
//		//{
//			TxMsg = Light_Sensor_Bar_Encoding();
//			Tx_MailBox = CAN_Transmit(CAN1, &TxMsg);							//transmit the message
//			while(CAN_TransmitStatus(CAN1,Tx_MailBox) != CANTXOK && retry--);
//		//}
//		//last_white_pos = white_pos;
//	//}
//}

//colour = target, light_colour = current led light
void CaptureCalibrationData(Colour colour, Colour light_colour){
	u8 id_array[16] = {10,11,12,13,0,1,2,3,4,5,6,7,14,15,8,9};
	u8 i = 0; u8 j = 0;
	switch(light_colour){
		case(RED):
			LED_Control(0,1,0);
			break;
		case(GREEN):
			LED_Control(1,0,0);
			break;
		case(BLUE):
			LED_Control(0,0,1);
			break;
		default:
			break;
	}
	_delay_ms(50);
	for (i = 0; i<16; ++i){
		Avg_ADC_val[light_colour][i] = 0;
	}
	for (i = 0; i < SAMPLETIMES ; i++){
		for(j = 0; j<16 ; ++j){
			Avg_ADC_val[light_colour][j] += (u32)ADC_val[j] ;
		}
	}
	for(j = 0; j<16; j++){
		Avg_ADC_val[light_colour][j] /= SAMPLETIMES;
	}
	switch(colour){
		case(RED):
			printf("RED Calibration\n\r");
			break;
		case(BLUE):
			printf("BLUE Calibration\n\r");
			break;
		case(YELLOW):
			printf("YELLOW Calibration\n\r");
			break;
		case(WHITE):
			printf("WHITE Calibration\n\r");
			break;
		default:
			break;
	}
	for(i = 0; i<16; i++){
		printf("Avg_ADC_val[%d][%d] = %d \n\r", light_colour, id_array[i], Avg_ADC_val[light_colour][id_array[i]]);
		switch(colour){
			case(RED):
				if(light_colour == RED){
					RED_target[i].R = (u16)Avg_ADC_val[light_colour][id_array[i]];
				}
				if(light_colour == GREEN){
					RED_target[i].G = (u16)Avg_ADC_val[light_colour][id_array[i]];
				}
				if(light_colour == BLUE){
					RED_target[i].B = (u16)Avg_ADC_val[light_colour][id_array[i]];
				}
				break;
			case(BLUE):
				if(light_colour == RED)
					BLUE_target[i].R = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == GREEN)
					BLUE_target[i].G = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == BLUE)
					BLUE_target[i].B = (u16)Avg_ADC_val[light_colour][id_array[i]];
				break;
			case(YELLOW):
				if(light_colour == RED)
					YELLOW_target[i].R = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == GREEN)
					YELLOW_target[i].G = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == BLUE)
					YELLOW_target[i].B = (u16)Avg_ADC_val[light_colour][id_array[i]];
				break;
			case(WHITE):
				if(light_colour == RED)
					WHITE_target[i].R = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == GREEN)
					WHITE_target[i].G = (u16)Avg_ADC_val[light_colour][id_array[i]];
				if(light_colour == BLUE)
					WHITE_target[i].B = (u16)Avg_ADC_val[light_colour][id_array[i]];
				break;
			default:
				break;
		}		
	}
	LED_Control(0,0,0);
	_delay_ms(50);
	GPIO_SetBits(GPIOA, FD10);
	GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	GPIO_SetBits(GPIOD, FD6);
	_delay_ms(25);
	GPIO_ResetBits(GPIOA, FD10);
	GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	GPIO_ResetBits(GPIOD, FD6);
	_delay_ms(50);
	GPIO_SetBits(GPIOA, FD10);
	GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	GPIO_SetBits(GPIOD, FD6);
	_delay_ms(25);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_ResetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
	writeFlash();
}

extern u8 enter_calibration;

//void Calibration_mode(){
//	u8 id_array[16] = {10,11,12,13,0,1,2,3,4,5,6,7,14,15,8,9};
//	u8 button_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
//	u8 button_prev_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
//	u8 step_counter = 0;
//	u16 timer = 300;
//	u16 ticks_img;
//	u16 colour_timer = 1000; // 1 sec for one type of light (total 3 seconds for each colour on the floor)
//	u32 i = 0;
//	u8 last_can_step = 16;
//	LED_Control(0,0,0);
//	printf("Calibration Mode Started.\n\r");	
//	button_prev_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
//	button_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
//	printf("Started.\n\r");
//	GPIO_SetBits(GPIOA, FD10);
//	GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
//	GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
//	GPIO_SetBits(GPIOD, FD6);

//	while(1){
//		if(ticks_img!=get_ticks()){
//			ticks_img=get_ticks();
//			if(ticks_img % 300 == 0){
//				button_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
//				if((button_prev_state != button_state) && button_state)	step_counter++;
//				button_prev_state = button_state;
//			}
//			if(last_can_step != can_calibration_step){
//				if(can_calibration_step == 1 || can_calibration_step == 3 || can_calibration_step == 5 || can_calibration_step == 7 || can_calibration_step == 8 ||  can_calibration_step == 10){
//					step_counter = can_calibration_step;
//					can_calibration_step++;
//				}
//				last_can_step = can_calibration_step;
//			}
//				
//			//IndicatorControl(step_counter);
//			switch(step_counter){
//				case(1): //first press, get R
//					printf("R.\n\r");
//					CaptureCalibrationData(RED, RED);
//					CaptureCalibrationData(RED, GREEN);
//					CaptureCalibrationData(RED, BLUE);
//					step_counter++;
//					for(i=0;i<16;i++){
//						printf("Calibration Done, RED[%d] %d %d %d\n\r",i, RED_target[i].R, RED_target[i].G, RED_target[i].B);
//					}
//					break;
//				case(2):
//					colour_timer = 1000;
//					break;
//				case(3): //2nd press	, get B
//					printf("B.\n\r");
//					colour_timer--;
//					CaptureCalibrationData(BLUE, RED);
//					CaptureCalibrationData(BLUE, GREEN);
//					CaptureCalibrationData(BLUE, BLUE);
//					step_counter++;
//					for(i=0;i<16;i++){
//						printf("BLUE %d %d %d\n\r", BLUE_target[i].R, BLUE_target[i].G, BLUE_target[i].B);
//					}
//					break;
//				case(4):
//					colour_timer = 1000;
//					break;
//				case(5): //3rd press	, get Y
//					printf("Y\n\r");
//					CaptureCalibrationData(YELLOW, RED);
//					CaptureCalibrationData(YELLOW, GREEN);
//					CaptureCalibrationData(YELLOW, BLUE);
//					step_counter++;
//					for(i=0;i<16;i++){
//						printf("YELLOW %d %d %d\n\r", YELLOW_target[i].R, YELLOW_target[i].G, YELLOW_target[i].B);
//					}
//					break;
//				case(6):
//					colour_timer = 1000;
//					break;
//				case(7): //4th press	, get W
//					printf("W.\n\r");
//					CaptureCalibrationData(WHITE, RED);
//					CaptureCalibrationData(WHITE, GREEN);
//					CaptureCalibrationData(WHITE, BLUE);
//					step_counter++;
//					for(i=0;i<16;i++){
//						printf("WHITE %d %d %d\n\r", WHITE_target[i].R, WHITE_target[i].G, WHITE_target[i].B);
//					}
//					break;
//				case(8):
//					GPIO_SetBits(GPIOA, FD10);
//					GPIO_SetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
//					GPIO_SetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
//					GPIO_SetBits(GPIOD, FD6);
//					LED_Control(0,0,0);
//					for(i=0;i<16;i++){
//						printf("Sensor %d\n\r", i);
//						printf("Calibration of RED - %d %d %d\n\r", RED_target[i].R, RED_target[i].G, RED_target[i].B);
//						printf("Calibration of BLUE - %d %d %d\n\r", BLUE_target[i].R, BLUE_target[i].G, BLUE_target[i].B);
//						printf("Calibration of YELLOW - %d %d %d\n\r", YELLOW_target[i].R, YELLOW_target[i].G, YELLOW_target[i].B);
//						printf("Calibration of WHITE - %d %d %d\n\r", WHITE_target[i].R, WHITE_target[i].G, WHITE_target[i].B);
//					}
//					step_counter++;
//					break;
//				case(9):
//					GPIO_ResetBits(GPIOA, FD10);
//					GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
//					GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
//					GPIO_ResetBits(GPIOD, FD6);
//					break;
//				case(10):
//					GPIO_ResetBits(GPIOA, FD10);
//					GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
//					GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
//					GPIO_ResetBits(GPIOD, FD6);
//					enter_calibration = 0;
//					return;
//				default:
//					LED_Control(0,0,0);					
//			}
//		}
//	}
//	enter_calibration = 0;
//}
