#include "flashMem.h"
#include "lightSensor.h"

extern Reading max_1;
uint32_t startAddress = 0x8009000; //0x8019000;

void writeFlash(void){
	u8 i;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	for(i = 0; i < 16; i++){
		FLASH_ProgramHalfWord((startAddress + (4*i)),max_1.off_reading[i]);
	}
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 64 + (4 * i)),max_1.red_reading[i]);
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 128 + (4 * i)),max_1.green_reading[i]);
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 192 + (4 * i)),max_1.blue_reading[i]);
    }
	FLASH_Lock();
}

void readFlash(void){
  u8 i;
    for(i=0; i<16; i++){
		max_1.off_reading[i] = *(uint16_t *)(startAddress + (4 * i));
		
	}
    for(i = 0 ; i < 16; i++){
        max_1.red_reading[i] = *(uint16_t *)(startAddress + 64 + (4 * i));
    }
    for(i = 0 ; i < 16; i++){
        max_1.green_reading[i] = *(uint16_t *)(startAddress + 128 + (4 * i));
    }
	
    for(i=0; i<16; i++){
		max_1.blue_reading[i] = *(uint16_t *)(startAddress + 192 + (4 * i));
	}
}
