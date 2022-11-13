//=====================================================================
//
// common.c                                              Date: 2022/11/3
//
// Author : Mustafa abdelhamid
//
//=====================================================================

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"

uint32_t getBit (uint32_t value , unsigned int b) {
	return (value >>b) &1 ; 
}


uint32_t getBits (uint32_t value , unsigned int hi,unsigned int lo) {
	return (value>>lo) & ((1<<(hi-lo+1))-1) ;
}

uint32_t concatenate (uint32_t value1 , unsigned int width1,uint32_t value2 , unsigned int width2) {
	assert((width1+width2) <=32);
	return (value1<<width2) | value2 ; 
}
