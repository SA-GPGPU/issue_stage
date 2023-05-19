//=====================================================================
//
// gpgpu.h                                              Date: 2022/11/11
//
// Author : Mustafa abdelhamid
//
//=====================================================================
/*
  Description:
=================
	1. it contains new defined data types used as input and output from gpgpu stage
		gpu_req_if, warp_ctl_if, gpu_commit_if
	2. Functions, used in gpgpu, prototype
		generate_mask: generate the taken and not taken masks
		control_tmask: generates the new thread mask
		control_wmask: generates the new warp mask and the PC
		control_split:
		control_bar:
		ex_gpgpu : All gpgpu execute unit model
*/
// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "define.h"





//			GPGPU types Structs
//===================================================================

typedef struct gpu_tmc_t {
    unsigned valid:1;
    unsigned tmask : NUM_THREADS ;
} gpu_tmc_t;

# define GPU_TMC_BITS 1+NUM_THREADS

typedef struct gpu_wspawn_t {
    unsigned valid:1;
    unsigned wmask : NUM_WARPS ;
    unsigned pc : 32;
} gpu_wspawn_t;

# define GPU_WSPAWN_BITS 1+32+NUM_WARPS

typedef struct gpu_split_t {
    unsigned valid:1;
    unsigned diverged:1;
    unsigned then_tmask : NUM_THREADS ;
    unsigned else_tmask : NUM_THREADS ;
    unsigned pc : 32;
} gpu_split_t;

# define GPU_SPLIT_BITS 2*NUM_THREADS+32+1+1

typedef struct gpu_barrier_t {
    unsigned valid:1;
    unsigned id : NB_BITS ;
    unsigned size_m1 : NW_BITS;
} gpu_barrier_t;

# define GPU_BARRIER_BITS 1+NB_BITS+NW_BITS


//======================================================================================

//				 GPGPU interfaces
//=======================================================================================

typedef struct gpu_req_if
{
    	//wire                    valid;

    	uint64_t uuid : UUID_BITS;
	unsigned tmask : NUM_THREADS ;
	unsigned wid : NUM_WARPS;

	unsigned pc : 32;
	unsigned next_PC : 32;

    	unsigned op_type : 4;
    	//unsigned op_mod: 3;
	unsigned tid: NUM_THREADS; // log2(NUM_THREADS)
	uint32_t rs1_data [NUM_THREADS] ;
	uint32_t rs2_data [NUM_THREADS] ;

	//uint32_t rs3_data [NUM_THREADS] ;
	//wire [`NR_BITS-1:0]     rd;
	//wire                    wb;
	//wire                    ready;
} gpu_req_if;

typedef struct warp_ctl_if
{
    	 unsigned valid:1;
         unsigned wid : NUM_WARPS;
         gpu_tmc_t tmc;
         gpu_wspawn_t wspawn;
         gpu_barrier_t barrier;
         gpu_split_t split;


} warp_ctl_if;


//============================= FUNCTIONS ============================================
void generate_mask (uint8_t thread_mask , uint32_t *rs1_data ,uint8_t *taken_mask ,uint8_t *nottaken_mask  ) ;
/*
========================================================================
	thread_mask 	||	input thread mask
	rs1_data	||  	is a pointer of an array of NUM_THREADS
	taken_mask	|| 	generated taken mask
	nottaken_mask	||	generated nottaken mask
=========================================================================
*/
void control_tmask (uint8_t is_pred, uint8_t thread_mask,uint32_t rs1_data_per_thread ,uint8_t taken_mask,uint8_t *valid,uint8_t *thread_mask_out) ;

void control_wmask (	uint32_t rs1_data_per_thread,uint32_t rs2_data_per_thread ,
			uint8_t *valid,uint8_t *warp_mask_out,uint32_t *pc_out);

void control_split (	uint8_t taken_mask ,uint8_t nottaken_mask ,
			uint8_t *valid,uint8_t *diverged,uint8_t *then_tmask,
			uint32_t *next_pc);

void control_bar (	uint32_t rs1_data_per_thread,uint32_t rs2_data_per_thread ,
			uint8_t *valid,uint8_t *bar_id,uint8_t *bar_size);

