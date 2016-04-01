#include "lrf_string.h"

u16 getlength(char str[]){
	u16 i,length=0;
	for(i=0;i<255;i++)
	{
		if(str[i] == '\0')return length;
		length++;
	}
	return 0;
}

void stringcpy(char* str1, char str2[]){
	u8 i;
	for(i=0;str2[i]!='\0' && i<255;i++) *(str1+i) = str2[i];
	*(str1+i) = '\0';
	return;
}

u16 stringcat(char* str1, char str2[]){
	u8 i,j;
	u16 length;
	length = getlength(str2);
	if (length==0) return 1;
	for(i=0;*(str1+i)!='\0';i++) if(i>255) return 1;
	for(j=0;j<length;j++)*(str1+i+j) = str2[j];
	*(str1+i+j) = '\0';
	return 0;
}

u16 power(u16 integer, u8 index){
	if(!index) return 1;
	return integer * power(integer, index-1);
}

void int2string(char* str, u16 integer, u8 length){
	u8 pos,offset;
	offset = 0x30;
	for (pos=0;pos<length;pos++){
		*(str+pos) = ((integer % power(10,length-pos)) / power(10,length-pos-1)) + offset;
	}
	*(str+pos) = '\0';
	return;
}
