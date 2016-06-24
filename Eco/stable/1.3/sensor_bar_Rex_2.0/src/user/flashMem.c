#include "flashMem.h"
#include "lightSensor.h"

extern Reading max_1;
uint32_t startAddress = 0x8009000; //0x8019000;

void writeFlash(void){
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	u32 current_address = startAddress;
	
	for(u8 i=0; i<REGIONS; i++){
		for (u8 j=0; j<3; j++){
			FLASH_ProgramHalfWord(current_address, compensated_region_color[i][j]);
			current_address += 4;
		}
	}
	
	for(u8 i=0; i<REGIONS; i++){
		for (u8 j=0; j<3; j++){
			FLASH_ProgramHalfWord(current_address, region_color_average[i][j]);
			current_address += 4;
		}
	}
	
	FLASH_Lock();
}

void readFlash(void){
	
	u32 current_address = startAddress;
	
	for(u8 i=0; i<REGIONS; i++){
		for (u8 j=0; j<3; j++){
			compensated_region_color[i][j] = *(uint16_t *)(current_address);
			current_address += 4;
		}
	}
	
	for(u8 i=0; i<REGIONS; i++){
		for (u8 j=0; j<3; j++){
			region_color_average[i][j] = *(uint16_t *)(current_address);
			current_address += 4;
		}
	}
}
