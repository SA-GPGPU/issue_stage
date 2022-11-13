#include <stdio.h>
#include <stdint.h>
#include "define.h"
#include "config.h"
#include "common.h"

#define DATAW   UUID_BITS + NUM_THREADS + 32 + EX_BITS + INST_OP_BITS + INST_FRM_BITS + 1 + (NR_BITS * 4) + 32 + 1 + 1
#define ADDRW    int_log2(IBUF_SIZE+1)		 // round up log2
#define NWARPSW  int_log2(NUM_WARPS+1)

typedef struct VX_gpr_req_if
{
    unsigned       wid:NW_BITS;
    unsigned       rs1:NR_BITS;
    unsigned       rs2:NR_BITS;
    unsigned       rs3:NR_BITS;


} VX_gpr_req_if;

typedef struct VX_gpr_rsp_if
{
    unsigned       rs1_data[NUM_THREADS];
    unsigned       rs2_data[NUM_THREADS];
    unsigned       rs3_data[NUM_THREADS];
} VX_gpr_rsp_if;

typedef struct VX_writeback_if
{
    uint64_t       uuid:UUID_BITS;
    bool       `   tmask[NUM_THREADS];
    unsigned       wid:NW_BITS;
    unsigned       PC:31;
    unsigned       rd:NR_BITS;
    unsigned       data[NUM_THREADS];
    unsigned       eop:1;
    unsigned       valid:1;


} VX_writeback_if;


typedef struct VX_ibuffer_if
{

    unsigned     wid: NW_BITS;
    unsigned     tmask: NUM_THREADS;
    unsigned     PC : 32;
    unsigned     ex_type: EX_BITS;
    unsigned     op_type: INST_OP_BITS;
    unsigned     op_mod: INST_MOD_BITS;
    signed       imm:32;
    unsigned     rd :NR_BITS;
    unsigned     rs1:NR_BITS;
    unsigned     rs2:NR_BITS;
    unsigned     rs3:NR_BITS;
    unsigned     valid:1;
    unsigned     uuid:1;
    unsigned     wb:1;
    unsigned     use_PC:1;
    unsigned     use_imm:1;


} VX_ibuffer_if;



typedef struct VX_decode_if
{
    uint64_t       uuid:UUID_BITS;
    unsigned       tmask:NUM_THREADS;
    unsigned       wid:NW_BITS;
    unsigned       PC:31;
    unsigned       rd:NR_BITS;
    unsigned       rs1:NR_BITS;
    unsigned       rs2:NR_BITS;
    unsigned       rs3:NR_BITS;
    signed         imm:32;
    unsigned       ex_type:EX_BITS;
    unsigned       op_type:INST_OP_BITS;
    unsigned       op_mod:INST_MOD_BITS;
    unsigned       valid:1;
    unsigned       wb:1;
    unsigned       use_imm:1;
    unsigned       use_PC:1;


} VX_decode_if;






//============================= FUNCTIONS ============================================
//
void ibuffer (VX_decode_if decode_if,bool ready_ibuffer,VX_ibuffer_if *ibuffer_if, bool *ready_decode, bool initial_flag) ;
void gpr_stage (VX_writeback_if VX_writeback_if, VX_gpr_req_if gpr_req_if, VX_gpr_rsp_if *gpr_rsp_if  ) ;
void scoreboard (VX_ibuffer_if ibuffer_if, VX_writeback_if writeback_if, bool *ready_ibuffer , bool initial_flag ) ;

