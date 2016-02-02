#include "flashsave.h"
#include "lightsensor.h"

uint32_t data_set[4][3] ={0}; //4 colour target, each have R G B
uint32_t startAddress = 0x8009000; //0x8019000;

extern Colour_Target RED_target[16];
extern Colour_Target BLUE_target[16];
extern Colour_Target YELLOW_target[16];
extern Colour_Target WHITE_target[16];

void writeFlash(void){
	u8 i,j;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	for(i=0; i<16; i++){
		FLASH_ProgramHalfWord((startAddress + (i*12+4)),RED_target[i].R);
		FLASH_ProgramHalfWord((startAddress + (i*12+8)),RED_target[i].G);
		FLASH_ProgramHalfWord((startAddress + (i*12+12)),RED_target[i].B);
	}
	for(i=0; i<16; i++){
		FLASH_ProgramHalfWord((startAddress + 192 +(i*12+4)),BLUE_target[i].R);
		FLASH_ProgramHalfWord((startAddress + 192 +(i*12+8)),BLUE_target[i].G);
		FLASH_ProgramHalfWord((startAddress + 192 + (i*12+12)),BLUE_target[i].B);
	}
	for(i=0; i<16; i++){
		FLASH_ProgramHalfWord((startAddress + 384 + (i*12+4)),YELLOW_target[i].R);
		FLASH_ProgramHalfWord((startAddress + 384 + (i*12+8)),YELLOW_target[i].G);
		FLASH_ProgramHalfWord((startAddress + 384 + (i*12+12)),YELLOW_target[i].B);
	}
	for(i=0; i<16; i++){
		FLASH_ProgramHalfWord((startAddress + 576 + (i*12+4)),WHITE_target[i].R);
		FLASH_ProgramHalfWord((startAddress + 576 + (i*12+8)),WHITE_target[i].G);
		FLASH_ProgramHalfWord((startAddress + 576 + (i*12+12)),WHITE_target[i].B);
	}
	FLASH_Lock();
}

//store data from FLASH to the array(memory)
void readFlash(void){
  u32 i, j = 0;
		/*	for(i=0; i<16; i++){
		printf("Sensor: %d	%d	", i, *(uint16_t *)(startAddress + (i*12+4)));
		printf("%d	", *(uint16_t *)(startAddress + (i*12+8)));
		printf("%d \n\r", *(uint16_t *)(startAddress + (i*12+12)));
		*/
	for(i=0; i<16; i++){
		RED_target[i].R = *(uint16_t *)(startAddress + (i*12+4));
		RED_target[i].G = *(uint16_t *)(startAddress + (i*12+8));
		RED_target[i].B = *(uint16_t *)(startAddress + (i*12+12));
	}
	for(i=0; i<16; i++){
		BLUE_target[i].R = *(uint16_t *)(startAddress + 192 +(i*12+4));
		BLUE_target[i].G = *(uint16_t *)(startAddress + 192 +(i*12+8));
		BLUE_target[i].B = *(uint16_t *)(startAddress + 192 +(i*12+12));
	}
	for(i=0; i<16; i++){
		YELLOW_target[i].R = *(uint16_t *)(startAddress + 384 +(i*12+4));
		YELLOW_target[i].G = *(uint16_t *)(startAddress + 384 +(i*12+8));
		YELLOW_target[i].B = *(uint16_t *)(startAddress + 384 +(i*12+12));
	}
	for(i=0; i<16; i++){
		WHITE_target[i].R = *(uint16_t *)(startAddress + 576 +(i*12+4));
		WHITE_target[i].G = *(uint16_t *)(startAddress + 576 +(i*12+8));
		WHITE_target[i].B = *(uint16_t *)(startAddress + 576 +(i*12+12));
	}
}
