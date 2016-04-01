/*-----------------------------------------------------------------------------
 * Name:    Board_LED.h
 * Purpose: LED interface header file
 * Rev.:    1.00
 *-----------------------------------------------------------------------------*/

#ifndef __BOARD_LED_H
#define __BOARD_LED_H

#include <stdint.h>

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Control all LEDs with the bit vector \em val
  \param[in]   val  each bit represents the status of one LED.
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of available LEDs on evaluation hardware
  \return      Number of available LEDs
*/

extern int32_t  LED_Initialize   (void);
extern int32_t  LED_Uninitialize (void);
extern int32_t  LED_On           (uint32_t num);
extern int32_t  LED_Off          (uint32_t num);
extern int32_t  LED_SetOut       (uint32_t val);
extern uint32_t LED_GetCount     (void);

#endif /* __BOARD_LED_H */
