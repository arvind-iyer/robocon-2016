#include "quick_math.h"

/******************************************************
** QUICK MATH - STM32F405VG
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** March 2016
**
** Speed is the heart of battle. ~Dark Seer
*******************************************************/

s16 get_median_of_five(s16 a, s16 b, s16 c, s16 d, s16 e){
    return b < a ? d < c ? b < d ? a < e ? a < d ? e < d ? e : d
                                                 : c < a ? c : a
                                         : e < d ? a < d ? a : d
                                                 : c < e ? c : e
                                 : c < e ? b < c ? a < c ? a : c
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : c < b ? c : b
                         : b < c ? a < e ? a < c ? e < c ? e : c
                                                 : d < a ? d : a
                                         : e < c ? a < c ? a : c
                                                 : d < e ? d : e
                                 : d < e ? b < d ? a < d ? a : d
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : d < b ? d : b
                 : d < c ? a < d ? b < e ? b < d ? e < d ? e : d
                                                 : c < b ? c : b
                                         : e < d ? b < d ? b : d
                                                 : c < e ? c : e
                                 : c < e ? a < c ? b < c ? b : c
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : c < a ? c : a
                         : a < c ? b < e ? b < c ? e < c ? e : c
                                                 : d < b ? d : b
                                         : e < c ? b < c ? b : c
                                                 : d < e ? d : e
                                 : d < e ? a < d ? b < d ? b : d
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : d < a ? d : a;
}

/**
** This function tries to cap a s16 within a specific range.
** The inline property tries to remove the function call and make it runs faster
**/
__inline int16_t s16_cap(int16_t in_num, int16_t upper_bound, int16_t lower_bound){
	return in_num>upper_bound?upper_bound:in_num<lower_bound?lower_bound:in_num;
}


/**
** This function tries to cap a u16 within a specific range.
** The inline property tries to remove the function call and make it runs faster
**/
__inline uint16_t u16_cap(uint16_t in_num, uint16_t upper_bound, uint16_t lower_bound){
	return in_num>upper_bound?upper_bound:in_num<lower_bound?lower_bound:in_num;
}

/**
** The followings are square root function for unsigned integer
** They uses Jack W. Crenshaw's integer square root algorithm. Let's thanks him.
** As sqrt(2^(2n)) = 2^n, so u16_sqrt returns u8, and u32_sqrt returns u16.
** Reference: http://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots
**/

__inline uint8_t u16_sqrt(uint16_t in_num){
	uint16_t root = 0;
	uint16_t remainder = 0;
	for (uint8_t i=0; i<8; i++){
		root <<= 1;
		remainder = (remainder<<2) + (in_num>>14);
		in_num <<= 2;
		root++;
		if (root <= remainder){
			remainder -= root;
			root++;
		}else{
			root--;
		}
	}
	return (uint8_t) (root>>1);
}

__inline uint16_t u32_sqrt(uint32_t in_num){
	uint32_t root = 0;
	uint32_t remainder = 0;
	for (uint8_t i=0; i<16; i++){
		root <<= 1;
		remainder = (remainder<<2) + (in_num>>30);
		in_num <<= 2;
		root++;
		if (root <= remainder){
			remainder -= root;
			root++;
		}else{
			root--;
		}
	}
	return (uint16_t) (root>>1);
}

void s16_quicksort(s16* list, u16 left, u16 right){
	
}
