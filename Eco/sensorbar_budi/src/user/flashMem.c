#include "flashMem.h"
#include "lightSensor.h"

extern Reading max_1;
extern u16 calibratedRedAverage[NUMOFAREAS];
extern u16 calibratedGreenAverage[NUMOFAREAS];
extern u16 calibratedBlueAverage[NUMOFAREAS];
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
        FLASH_ProgramHalfWord(currentAddress,calibratedRedAverage[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 6 ; i++){
        FLASH_ProgramHalfWord(currentAddress,calibratedGreenAverage[i]);
        currentAddress += 4;
    }
    
    for(i = 0 ; i < 6 ; i++){
        FLASH_ProgramHalfWord(currentAddress,calibratedBlueAverage[i]);
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
        calibratedRedAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
    
    for(i = 0 ; i < (u8)NUMOFAREAS ; i++){
        calibratedGreenAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
    
    for(i = 0 ; i < (u8)NUMOFAREAS ; i++){
        calibratedBlueAverage[i] = *(uint16_t *)currentAddress;
        currentAddress += 4;
    }
}
