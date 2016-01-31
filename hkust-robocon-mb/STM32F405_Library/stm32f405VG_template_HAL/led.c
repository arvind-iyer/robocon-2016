/*-----------------------------------------------------------------------------
 * Name:    led.c
 * Purpose: LED interface for MCBSTM32F400 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/


#include "stm32f4xx_hal.h"

/* GPIO Pin identifier */
typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} GPIO_PIN;

/* LED GPIO Pins */
static const GPIO_PIN LED_PIN[] = {
  { GPIOE, GPIO_PIN_2  },
  { GPIOE, GPIO_PIN_3  },
  { GPIOE, GPIO_PIN_4  },
  { GPIOE, GPIO_PIN_5  },
  { GPIOE, GPIO_PIN_6  }
};

#define LED_COUNT (sizeof(LED_PIN)/sizeof(GPIO_PIN))


/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();

  GPIO_InitStruct.Pin   = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {

  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {
  HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_SET);
  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {
  HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_RESET);
  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut (uint32_t val) {
  uint32_t n;

  for (n = 0; n < LED_COUNT; n++) {
    if (val & (1 << n)) LED_On (n);
    else                LED_Off(n);
  }
  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return LED_COUNT;
}
