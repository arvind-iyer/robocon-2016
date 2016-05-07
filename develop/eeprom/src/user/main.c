/**
  ******************************************************************************
  * @file    EEPROM_Emulation/src/main.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/** @addtogroup EEPROM_Emulation
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"

#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus  HSEStartUpStatus;
FLASH_Status FlashStatus;
uint16_t VarValue = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NumbOfVar] = {0x5555, 0x6666, 0x7777};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

u32 this_loop_ticks = 0;
u8 init = 0;
u8 state = 0;
u16 pwm = 2048;

uint16_t test;

int main(void) {
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	can_init();
	can_rx_init();
	can_motor_init();
	uart_init(COM1, 115200);
	pneumatic_init();
	buzzer_init();
	button_init();
	
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();

  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();

  /* EEPROM Init */
  EE_Init();

 
	tft_put_logo(85, 120);            
	CONTROL_STATE last_control_state = MANUAL_MODE;
		
	while(1){
		this_loop_ticks = get_full_ticks();
		button_update();
		
		tft_clear();
		tft_prints(0,0,"[EEPROM Demo]");
		tft_prints(0,6,"(1) Load val");
		tft_prints(0,7,"(2) Clear val");
		tft_prints(0,9,"by Pang");
		
		EE_ReadVariable(VirtAddVarTab[0], &test);
		tft_prints(0,2,"Val: %d",test);
		
		if(button_pressed(BUTTON_1)) {
			EE_WriteVariable(VirtAddVarTab[0], 69);
			tft_prints(0,3,"Written");
		}
		
		if(button_pressed(BUTTON_2)) {
			EE_WriteVariable(VirtAddVarTab[0], 0);
			tft_prints(0,3,"Cleared");
		}
		
		tft_update();
	}
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif