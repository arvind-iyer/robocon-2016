#include "flashMem.h"
#include "lightSensor.h"

extern Reading max_1;
extern u16 calibratedHueAverage[NUMOFAREAS];
extern u16 calibratedSaturationAverage[NUMOFAREAS];
extern u16 calibratedValueAverage[NUMOFAREAS];
uint32_t startAddress = 0x8009000; //0x8019000;

void writeFlash(void){
	u8 i;
    uint32_t currentAddress = startAddress;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	for(i = 0; i < 16; i++){
		FLASH_ProgramHalfWord(currentAddress,max_1.off_reading[i]);
        currentAddress += 4;
	}
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord(currentAddress,max_1.red_reading[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord(currentAddress,max_1.green_reading[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord(currentAddress,max_1.blue_reading[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 6 ; i++){
        FLASH_ProgramHalfWord(currentAddress,calibratedHueAverage[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 6 ; i++){
        FLASH_ProgramHalfWord(currentAddress,calibratedSaturationAverage[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 6 ; i++){
        FLASH_ProgramHalfWord(currentAddress,calibratedValueAverage[i]);
        currentAddress += 4;
    }
    
	FLASH_Lock();
}

void readFlash(void){
    u8 i;
    uint32_t currentAddress = startAddress;
    for(i=0; i<16; i++){
		max_1.off_reading[i] = *(uint16_t *)currentAddress;
		currentAddress += 4;
	}
    for(i = 0 ; i < 16; i++){
        max_1.red_reading[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
    for(i = 0 ; i < 16; i++){
        max_1.green_reading[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }	
    for(i = 0; i < 16; i++){
		max_1.blue_reading[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
	}
    for(i = 0 ; i < (u8)NUMOFAREAS ; i++){
        calibratedHueAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
    
    for(i = 0 ; i < (u8)NUMOFAREAS ; i++){
        calibratedSaturationAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
    
    for(i = 0 ; i < (u8)NUMOFAREAS ; i++){
        calibratedValueAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
}
