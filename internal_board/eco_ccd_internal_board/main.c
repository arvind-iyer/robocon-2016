#include "main.h"

//Initialize GPIO for CCD LED
void ccdLed_init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
}

int main() {
	tft_init(2, BLACK, WHITE, GREEN);
	button_init();
	ticks_init();
    servo_init();
	linear_ccd_init();
	adc_init();
    LED_INIT();
    //ccdLed_init();

	long lastTick = get_ms_ticks();
    
	u32 prev_linear_ccd1[128];

    u32 prev_linear_ccd_buffer1[128];

	while (1) {

		for (u16 i=0;i<128;i++){
			tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
		}
		linear_ccd_read();
		for (u16 i=0;i<128;i++){
			tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
		}

	}

	return 0;
}