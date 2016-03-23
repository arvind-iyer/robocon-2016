#include "flashsave.h"

uint32_t data_set[list_num][3] ={0}; //4 colour target, each have R G B
uint32_t startAddress = 0x8009000; //0x8019000;

extern struct Reading* colour_list_ptr;

void writeFlash(void)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	for(u8 i=0;i<list_num;i++)
	{
		for(u8 j=0; j<16; j++)
		{
			FLASH_ProgramHalfWord((startAddress + 192*i+(j*12+4)),(colour_list_ptr + i)->red_reading[j]);
			FLASH_ProgramHalfWord((startAddress + 192*i+(j*12+8)),(colour_list_ptr + i)->blue_reading[j]);
			FLASH_ProgramHalfWord((startAddress + 192*i+(j*12+12)),(colour_list_ptr + i)->green_reading[j]);
		}
	}
	FLASH_Lock();
}

//store data from FLASH to the array(memory)
void readFlash(void)
{
		/*	for(i=0; i<16; i++){
		printf("Sensor: %d	%d	", i, *(uint16_t *)(startAddress + (i*12+4)));
		printf("%d	", *(uint16_t *)(startAddress + (i*12+8)));
		printf("%d \n\r", *(uint16_t *)(startAddress + (i*12+12)));
		*/
	for(u8 i=0;i<list_num;i++)
	{
		for(u8 j=0; j<16; j++)
		{
			(colour_list_ptr + i)->red_reading[j] = *(uint16_t *)(startAddress + 192*i + (j*12+4));
			(colour_list_ptr + i)->green_reading[j] = *(uint16_t *)(startAddress + 192*i + (j*12+8));
			(colour_list_ptr + i)->blue_reading[j] = *(uint16_t *)(startAddress + 192*i + (j*12+12));
		}
	}
}
