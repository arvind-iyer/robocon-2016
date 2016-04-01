/* string library for laser range finder */
#ifndef _STRING_H_
#define _STRING_H_
#include "stm32f4xx.h"

u16 getlength(char str[]);
u16 stringcat(char* str1, char str2[]);
void stringcpy(char* str1, char str2[]);
u16 power(u16 integer,u8 index);
void int2string(char* str, u16 integer,u8 length);

#endif /* _STRING_H_ */
