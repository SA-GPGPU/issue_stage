//=====================================================================
//
// common.h                                              Date: 2022/11/3
//
// Author : Mustafa abdelhamid
//
//=====================================================================
/*
  Description:
=================

	Common Functions, used in raiv model
		getBit : returns value [b]
		gitBits: returns value [hi:lo]
		concatenate: return two signals concatenated

*/
// -------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <stdint.h>

uint32_t getBit (uint32_t value , unsigned int b) ;

uint32_t getBits (uint32_t value , unsigned int hi,unsigned int lo) ;

uint32_t concatenate (uint32_t value1 , unsigned int width1,uint32_t value2 , unsigned int width2) ;
