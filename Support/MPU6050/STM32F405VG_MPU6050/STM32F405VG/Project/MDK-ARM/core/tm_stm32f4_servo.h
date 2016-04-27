
#ifndef TM_SERVO_H
#define TM_SERVO_H 110

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#include "tm_stm32f4_pwm.h"
#include "tm_stm32f4_timer_properties.h"
#include "lcd_main.h"

/**
 * @brief  Minimal pulse length for servo in micros
 */
#ifndef SERVO_MICROS_MIN
#define SERVO_MICROS_MIN	1000
#endif

/**
 * @brief  Maximal pulse length for servo in micros
 */
#ifndef SERVO_MICROS_MAX
#define SERVO_MICROS_MAX	2000
#endif

	
/**
 * @brief  Servo structure	
 */
typedef struct {
	TM_PWM_TIM_t PWM;           /*!< PWM settings */
	TIM_TypeDef* TIM;           /*!< Pointer to specific timer you will use for servo */
	TM_PWM_Channel_t Channel;   /*!< Output channel on specific timer */
	TM_PWM_PinsPack_t Pinspack; /*!< Pinspack for specific channel */
	float Degrees;              /*!< Position of servo in degrees */
	uint16_t Micros;            /*!< Pulse length in micro seconds for current servo position */
} TM_SERVO_t;

/**
 * @brief Results enumeration	
 */
typedef enum {
	TM_SERVO_Result_Ok = 0, /*!< Everything OK */
	TM_SERVO_Result_Error   /*!< An error occurred somewhere */
} TM_SERVO_Result_t;

/**
 * @brief  Initializes TIM and PWM for servo motor purpose
 * @param  *ServoStruct: Pointer to an empty @ref TM_SERVO_t servo structure
 * @param  *TIMx: Pointer to TIMx you will use for servo
 * @param  PWMChannel: Channel you will use for timer. This parameter can be a value of @ref TM_PWM_Channel_t enumeration
 * @param  Pinspack: Pinspack for channel. This parameter can be a value of @ref TM_PWM_PinsPack_t enumeration
 * @retval Member of TM_SERVO_Result_t
 */
TM_SERVO_Result_t TM_SERVO_Init(TM_SERVO_t* ServoStruct, TIM_TypeDef* TIMx, TM_PWM_Channel_t PWMChannel, TM_PWM_PinsPack_t Pinspack);

/**
 * @brief  Set rotation degrees for servo
 * @note   Degrees can be between 0 and 180
 * @param  *ServoStruct: Pointer to an @ref TM_SERVO_t servo structure
 * @param  degrees: Rotation in degrees, between 0 and 180
 * @retval Member of TM_SERVO_Result_t
 */
TM_SERVO_Result_t TM_SERVO_SetDegrees(TM_SERVO_t* ServoStruct, float degrees);

/**
 * @brief  Sets pulse length in microseconds
 * @param  *ServoStruct: Pointer to an @ref TM_SERVO_t servo structure
 * @param  micros: pulse length in microseconds
 * @retval Member of TM_SERVO_Result_t
 */
TM_SERVO_Result_t TM_SERVO_SetMicros(TM_SERVO_t* ServoStruct, uint16_t micros);


#endif
