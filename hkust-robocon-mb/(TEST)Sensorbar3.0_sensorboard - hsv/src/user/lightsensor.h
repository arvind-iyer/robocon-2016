#ifndef _LIGHTSENSOR_H
#define _LIGHTSENSOR_H

#include "stm32f10x.h"
#include "main.h"
#include "can_protocol.h"
#include "led.h"
#include "approx_math.h"
#include "stm32f10x_flash.h"

#define SAMPLETIMES	60 //20 //100

typedef enum {
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	EMPTY = 3,
	//only for result
	YELLOW = 4,
	WHITE = 5
} Colour;

typedef struct {
	u16 R;
	u16 G;
	u16 B;
} Colour_Target;

//u8 resultColour(u32 R, u32 G, u32 B);
void ColourTarget_Init(void);

void SwitchLight(void);

void CaptureData(void);
void AnalysisData(void);
u8 resultColour(u8 id, Colour_Target current);
void ShowWhiteLinePos(void);

void DataTransmit(void);

void CaptureCalibrationData(Colour,Colour);
void Calibration_mode(void);


#endif
