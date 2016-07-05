/**
	A gpio library for easier usage of GPIO ports
	Refreshed in F4 by Rex Cheng
*/

#include "gpio.h"

const GPIO 	
			/*** GPIOA ***/
			PA0 = {GPIOA, GPIO_Pin_0},
			PA1 = {GPIOA, GPIO_Pin_1},
			PA2 = {GPIOA, GPIO_Pin_2},
			PA3 = {GPIOA, GPIO_Pin_3},
			PA4 = {GPIOA, GPIO_Pin_4},
			PA5 = {GPIOA, GPIO_Pin_5},
			PA6 = {GPIOA, GPIO_Pin_6},
			PA7 = {GPIOA, GPIO_Pin_7},
			PA8 = {GPIOA, GPIO_Pin_8},
			PA9 = {GPIOA, GPIO_Pin_9},
			PA10 = {GPIOA, GPIO_Pin_10},
			PA11 = {GPIOA, GPIO_Pin_11},
			PA12 = {GPIOA, GPIO_Pin_12},

			/*** GPIOB ***/
			PB0 = {GPIOB, GPIO_Pin_0},
			PB1 = {GPIOB, GPIO_Pin_1},
			PB2 = {GPIOB, GPIO_Pin_2},
			PB3 = {GPIOB, GPIO_Pin_3},
			PB4 = {GPIOB, GPIO_Pin_4},
			PB5 = {GPIOB, GPIO_Pin_5},
			PB6 = {GPIOB, GPIO_Pin_6},
			PB7 = {GPIOB, GPIO_Pin_7},
			PB8 = {GPIOB, GPIO_Pin_8},
			PB9 = {GPIOB, GPIO_Pin_9},
			PB10 = {GPIOB, GPIO_Pin_10},
			PB11 = {GPIOB, GPIO_Pin_11},
			PB12 = {GPIOB, GPIO_Pin_12},
			PB13 = {GPIOB, GPIO_Pin_13},
			PB14 = {GPIOB, GPIO_Pin_14},
			PB15 = {GPIOB, GPIO_Pin_15},

			/*** GPIOC ***/
			PC0 = {GPIOC, GPIO_Pin_0},
			PC1 = {GPIOC, GPIO_Pin_1},
			PC2 = {GPIOC, GPIO_Pin_2},
			PC3 = {GPIOC, GPIO_Pin_3},
			PC4 = {GPIOC, GPIO_Pin_4},
			PC5 = {GPIOC, GPIO_Pin_5},
			PC6 = {GPIOC, GPIO_Pin_6},
			PC7 = {GPIOC, GPIO_Pin_7},
			PC8 = {GPIOC, GPIO_Pin_8},
			PC9 = {GPIOC, GPIO_Pin_9},
			PC10 = {GPIOC, GPIO_Pin_10},
			PC11 = {GPIOC, GPIO_Pin_11},
			PC12 = {GPIOC, GPIO_Pin_12},
			PC13 = {GPIOC, GPIO_Pin_13},
			PC14 = {GPIOC, GPIO_Pin_14},
			PC15 = {GPIOC, GPIO_Pin_15},
			
			/*** GPIOD ***/
			PD0 = {GPIOD, GPIO_Pin_0},
			PD1 = {GPIOD, GPIO_Pin_1},
			PD2 = {GPIOD, GPIO_Pin_2},
			PD3 = {GPIOD, GPIO_Pin_3},
			PD4 = {GPIOD, GPIO_Pin_4},
			PD5 = {GPIOD, GPIO_Pin_5},
			PD6 = {GPIOD, GPIO_Pin_6},
			PD7 = {GPIOD, GPIO_Pin_7},
			PD8 = {GPIOD, GPIO_Pin_8},
			PD9 = {GPIOD, GPIO_Pin_9},
			PD10 = {GPIOD, GPIO_Pin_10},
			PD11 = {GPIOD, GPIO_Pin_11},
			PD12 = {GPIOD, GPIO_Pin_12},
			PD13 = {GPIOD, GPIO_Pin_13},
			PD14 = {GPIOD, GPIO_Pin_14},
			PD15 = {GPIOD, GPIO_Pin_15},

			/*** GPIOE ***/
			PE0 = {GPIOE, GPIO_Pin_0},
			PE1 = {GPIOE, GPIO_Pin_1},
			PE2 = {GPIOE, GPIO_Pin_2},
			PE3 = {GPIOE, GPIO_Pin_3},
			PE4 = {GPIOE, GPIO_Pin_4},
			PE5 = {GPIOE, GPIO_Pin_5},
			PE6 = {GPIOE, GPIO_Pin_6},
			PE7 = {GPIOE, GPIO_Pin_7},
			PE8 = {GPIOE, GPIO_Pin_8},
			PE9 = {GPIOE, GPIO_Pin_9},
			PE10 = {GPIOE, GPIO_Pin_10},
			PE11 = {GPIOE, GPIO_Pin_11},
			PE12 = {GPIOE, GPIO_Pin_12},
			PE13 = {GPIOE, GPIO_Pin_13},
			PE14 = {GPIOE, GPIO_Pin_14},
			PE15 = {GPIOE, GPIO_Pin_15},

			/*** GPIOF ***/
			PF0 = {GPIOF, GPIO_Pin_0},
			PF1 = {GPIOF, GPIO_Pin_1},
			PF2 = {GPIOF, GPIO_Pin_2},
			PF3 = {GPIOF, GPIO_Pin_3},
			PF4 = {GPIOF, GPIO_Pin_4},
			PF5 = {GPIOF, GPIO_Pin_5},
			PF6 = {GPIOF, GPIO_Pin_6},
			PF7 = {GPIOF, GPIO_Pin_7},
			PF8 = {GPIOF, GPIO_Pin_8},
			PF9 = {GPIOF, GPIO_Pin_9},
			PF10 = {GPIOF, GPIO_Pin_10},
			PF11 = {GPIOF, GPIO_Pin_11},
			PF12 = {GPIOF, GPIO_Pin_12},
			PF13 = {GPIOF, GPIO_Pin_13},
			PF14 = {GPIOF, GPIO_Pin_14},
			PF15 = {GPIOF, GPIO_Pin_15},

			/*** GPIOG ***/
			PG0 = {GPIOG, GPIO_Pin_0},
			PG1 = {GPIOG, GPIO_Pin_1},
			PG2 = {GPIOG, GPIO_Pin_2},
			PG3 = {GPIOG, GPIO_Pin_3},
			PG4 = {GPIOG, GPIO_Pin_4},
			PG5 = {GPIOG, GPIO_Pin_5},
			PG6 = {GPIOG, GPIO_Pin_6},
			PG7 = {GPIOG, GPIO_Pin_7},
			PG8 = {GPIOG, GPIO_Pin_8},
			PG9 = {GPIOG, GPIO_Pin_9},
			PG10 = {GPIOG, GPIO_Pin_10},
			PG11 = {GPIOG, GPIO_Pin_11},
			PG12 = {GPIOG, GPIO_Pin_12},
			PG13 = {GPIOG, GPIO_Pin_13},
			PG14 = {GPIOG, GPIO_Pin_14},
			PG15 = {GPIOG, GPIO_Pin_15},
			
			/*** GPIOI ***/
			PI0 = {GPIOI, GPIO_Pin_0},
			PI1 = {GPIOI, GPIO_Pin_1},
			PI2 = {GPIOI, GPIO_Pin_2},
			PI3 = {GPIOI, GPIO_Pin_3},
			PI4 = {GPIOI, GPIO_Pin_4},
			PI5 = {GPIOI, GPIO_Pin_5},
			PI6 = {GPIOI, GPIO_Pin_6},
			PI7 = {GPIOI, GPIO_Pin_7},
			PI8 = {GPIOI, GPIO_Pin_8},
			PI9 = {GPIOI, GPIO_Pin_9},
			PI10 = {GPIOI, GPIO_Pin_10},
			PI11 = {GPIOI, GPIO_Pin_11},
			PI12 = {GPIOI, GPIO_Pin_12},
			PI13 = {GPIOI, GPIO_Pin_13},
			PI14 = {GPIOI, GPIO_Pin_14},
			PI15 = {GPIOI, GPIO_Pin_15},
			
			/*** GPIOJ ***/
			PJ0 = {GPIOJ, GPIO_Pin_0},
			PJ1 = {GPIOJ, GPIO_Pin_1},
			PJ2 = {GPIOJ, GPIO_Pin_2},
			PJ3 = {GPIOJ, GPIO_Pin_3},
			PJ4 = {GPIOJ, GPIO_Pin_4},
			PJ5 = {GPIOJ, GPIO_Pin_5},
			PJ6 = {GPIOJ, GPIO_Pin_6},
			PJ7 = {GPIOJ, GPIO_Pin_7},
			PJ8 = {GPIOJ, GPIO_Pin_8},
			PJ9 = {GPIOJ, GPIO_Pin_9},
			PJ10 = {GPIOJ, GPIO_Pin_10},
			PJ11 = {GPIOJ, GPIO_Pin_11},
			PJ12 = {GPIOJ, GPIO_Pin_12},
			PJ13 = {GPIOJ, GPIO_Pin_13},
			PJ14 = {GPIOJ, GPIO_Pin_14},
			PJ15 = {GPIOJ, GPIO_Pin_15},
			
			/*** GPIOK ***/
			PK0 = {GPIOK, GPIO_Pin_0},
			PK1 = {GPIOK, GPIO_Pin_1},
			PK2 = {GPIOK, GPIO_Pin_2},
			PK3 = {GPIOK, GPIO_Pin_3},
			PK4 = {GPIOK, GPIO_Pin_4},
			PK5 = {GPIOK, GPIO_Pin_5},
			PK6 = {GPIOK, GPIO_Pin_6},
			PK7 = {GPIOK, GPIO_Pin_7},
			PK8 = {GPIOK, GPIO_Pin_8},
			PK9 = {GPIOK, GPIO_Pin_9},
			PK10 = {GPIOK, GPIO_Pin_10},
			PK11 = {GPIOK, GPIO_Pin_11},
			PK12 = {GPIOK, GPIO_Pin_12},
			PK13 = {GPIOK, GPIO_Pin_13},
			PK14 = {GPIOK, GPIO_Pin_14},
			PK15 = {GPIOK, GPIO_Pin_15}
			
			;

/**
	@brief GPIO Pin initailizer
	@param gpio: A pointer to a gpio port, like &PE0
	
	@param mode: Operating mode with type @ref GPIOMode_TypeDef
		@ref GPIO_Mode_IN   		= 0x00 		< GPIO Input Mode 
		@ref GPIO_Mode_OUT  		= 0x01 		< GPIO Output Mode 
		@ref GPIO_Mode_AF   		= 0x02 		< GPIO Alternate function Mode 
		@ref GPIO_Mode_AN   		= 0x03 		< GPIO Analog Mode 
			
	@param speed: GPIO speed with type @ref GPIOSpeed_TypeDef
		@ref GPIO_Low_Speed     	= 0x00 		< legacy = GPIO_Speed_2MHz
		@ref GPIO_Medium_Speed  	= 0x01 		< legacy = GPIO_Speed_25MHz
		@ref GPIO_Fast_Speed    	= 0x02 		< legacy = GPIO_Speed_50MHz
		@ref GPIO_High_Speed    	= 0x03 		< legacy = GPIO_Speed_100MHz
			
	@param io_type: Operating output type with type @ref GPIOOType_TypeDef
		@ref GPIO_OType_PP 		= 0x00		< Push-Pull
		@ref GPIO_OType_OD 		= 0x01		< Open-Drain
			
	@param pp_type: Specify the operating pull-up/pull-down with type @ref GPIOPuPd_TypeDef
		@ref GPIO_PuPd_NOPULL 	= 0x00		< Nothing
		@ref GPIO_PuPd_UP     	= 0x01		< Pull up
		@ref GPIO_PuPd_DOWN   	= 0x02		< Pull down
			
	@param rcc_init: True if the GPIO port rcc also needs to be initialized
*/
void gpio_init(const GPIO* gpio, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed, GPIOOType_TypeDef io_type, GPIOPuPd_TypeDef pp_type, bool rcc_init){
	if (rcc_init) {gpio_rcc_init(gpio);}
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpio->gpio_pin;
	GPIO_InitStructure.GPIO_Speed = speed;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = io_type;
	GPIO_InitStructure.GPIO_PuPd = pp_type;
	
	GPIO_Init(gpio->gpio, &GPIO_InitStructure);
}

/**
	* @brief GPIO Real-time Clock Initialization
	* @param GPIO pointer
	* @retval None.
	*/
void gpio_rcc_init(const GPIO* gpio){
	switch ((u32) gpio->gpio) {
		
		case ((u32)GPIOA):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		break;
		
		case ((u32)GPIOB):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		break;
		
		case ((u32)GPIOC):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		break;
		
		case ((u32)GPIOD):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		break;
		
		case ((u32)GPIOE):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		break;
		
		case ((u32)GPIOF):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		break;
		
		case ((u32)GPIOG):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		break;
		
		case ((u32)GPIOI):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
		break;
				
		case ((u32)GPIOJ):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, ENABLE);
		break;
		
		case ((u32)GPIOK):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE);
		break;
	}
}

/**
	* @brief Read GPIO input value
	* @param GPIO pointer
	* @retval The GPIO Pin input value
	*/
u8 gpio_read_input(const GPIO* gpio){
	return GPIO_ReadInputDataBit(gpio->gpio, gpio->gpio_pin);
}

/**
	* @brief Read GPIO output value
	* @param GPIO pointer
	* @retval The GPIO Pin output value
	*/
u8 gpio_read_output(const GPIO* gpio){
	return GPIO_ReadOutputDataBit(gpio->gpio, gpio->gpio_pin);
}

/**
	* @brief Write GPIO value
	* @param GPIO pointer
	* @retval None
	*/
void gpio_write(const GPIO* gpio, BitAction BitVal){
	GPIO_WriteBit(gpio->gpio, gpio->gpio_pin, BitVal);
}

/**
	* @brief Toggle GPIO
	* @param GPIO pointer
	* @retval None
	*/
void gpio_toggle(const GPIO* gpio) {
	GPIO_WriteBit(gpio->gpio, gpio->gpio_pin, (BitAction) GPIO_ReadOutputDataBit(gpio->gpio, gpio->gpio_pin));
}
