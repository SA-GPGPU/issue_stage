//=====================================================================
//
// decoder.h                                              Date: 2022/10/29
//
// Author : Mustafa abdelhamid
//
//=====================================================================
/*
  Description:
=================
	1. it contains new defined data types used as input and output from decode stage
		instr_fetch_rsp, perf_decode_if, decode_if , wstall_if, join_if
	2. Functions, used in decoder, prototype
		signExtend : extend a signal of width [1 --> 16] to a 32 bit signal
		decode: raiv decoder model function
		get_regName: take register number and return its ABI name
*/
// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "define.h"


#define DEBUG



typedef struct instr_fetch_rsp
{
    	//unsigned valid : 1;
    	//unsigned ready : 1;

    	uint64_t uuid : UUID_BITS;
	unsigned tmask : NUM_THREADS ;

	unsigned wid : NUM_WARPS;

	unsigned pc : 32;
    	unsigned instr : 32;

} instr_fetch_rsp;

typedef struct perf_decode_if
{
    	uint64_t loads : PERF_CTR_BITS;
    	uint64_t stores : PERF_CTR_BITS;

    	uint64_t branches : PERF_CTR_BITS;


} perf_decode_if;


typedef struct wstall_if
{
    unsigned valid : 1;
    unsigned wid : NUM_WARPS;
    unsigned stalled : 1;

} wstall_if;

typedef struct join_if
{
    unsigned valid : 1;
    unsigned wid : NUM_WARPS;

} join_if;



//============================= FUNCTIONS ============================================
char *get_regName(uint16_t regNum) ;

uint32_t extendSign ( uint32_t signal , uint8_t signalWidth) ;

void decode (instr_fetch_rsp fetched_instruction, perf_decode_if *decoder_performance, VX_decode *decoded_signals , wstall_if *stalled_warps , join_if *join_inst ) ;

