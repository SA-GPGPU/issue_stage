#include <stdio.h>
#include <stdint.h>
#include "define.h"
#include "config.h"
#include "common.h"

#define DATAW   UUID_BITS + NUM_THREADS + 32 + EX_BITS + INST_OP_BITS + INST_FRM_BITS + 1 + (NR_BITS * 4) + 32 + 1 + 1
#define ADDRW    int_log2(IBUF_SIZE+1)		 // round up log2
#define NWARPSW  int_log2(NUM_WARPS+1)

#define DATAW_alu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (2 * NUM_THREADS * 32)
#define DATAW_lsu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32) + 1
#define DATAW_csr   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS + 32
#define DATAW_fpu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (3 * NUM_THREADS * 32)
#define DATAW_gpu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (3 * NUM_THREADS * 32)

typedef struct VX_gpu_req_if
{
    uint64_t    	uuid:UUID_BITS;
    bool            valid;
    unsigned    	wid:NW_BITS;
    bool  		 	tmask[NUM_THREADS];
    unsigned        PC:32;
    unsigned        next_PC:32;
    unsigned 		op_type:INST_GPU_BITS;
    unsigned		op_mod:INST_MOD_BITS;
    unsigned    	tid:NT_BITS;
    unsigned       	rs1_data[NUM_THREADS];
    unsigned       	rs2_data[NUM_THREADS];
    unsigned       	rs3_data[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_gpu_req_if;

typedef struct VX_fpu_req_if
{
    uint64_t    	uuid:UUID_BITS;
    bool            valid;
    unsigned    	wid:NW_BITS;
    bool  		 	tmask[NUM_THREADS];
    unsigned        PC:32;
    unsigned 		op_type:INST_FPU_BITS;
    unsigned		op_mod:INST_MOD_BITS;
    unsigned       	rs1_data[NUM_THREADS];
    unsigned       	rs2_data[NUM_THREADS];
    unsigned       	rs3_data[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_fpu_req_if;

typedef struct VX_csr_req_if
{
    uint64_t    	uuid:UUID_BITS;
    bool            valid;
    unsigned    	wid:NW_BITS;
    bool  		 	tmask[NUM_THREADS];
    unsigned        PC:32;
    unsigned 		op_type:INST_CSR_BITS;
    unsigned		addr:CSR_ADDR_BITS;
    bool            use_imm;
    signed		    imm:32;
    unsigned       	rs1_data[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_csr_req_if;

typedef struct VX_lsu_req_if
{
    uint64_t    	uuid:UUID_BITS;
    bool            valid;
    bool            is_fence;
    bool            is_prefetch;
    unsigned    	wid:NW_BITS;
    bool  		 	tmask[NUM_THREADS];
    unsigned        PC:32;
    unsigned        offset:32;
    unsigned 		op_type:INST_LSU_BITS;
    unsigned       	store_data[NUM_THREADS];
    unsigned       	base_addr[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_lsu_req_if;

typedef struct VX_alu_req_if
{
    uint64_t    	uuid:UUID_BITS;
    bool            valid;
    unsigned    	wid:NW_BITS;
    bool  		 	tmask[NUM_THREADS];
    unsigned        PC:32;
    unsigned        next_PC:32;
    unsigned 		op_type:INST_ALU_BITS;
    unsigned		op_mod:INST_MOD_BITS;
    bool            use_PC;
    bool            use_imm;
    signed	    	imm:32;
    unsigned    	tid:NT_BITS;
    unsigned       	rs1_data[NUM_THREADS];
    unsigned       	rs2_data[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_alu_req_if;


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
    bool           tmask[NUM_THREADS];
    unsigned       wid:NW_BITS;
    unsigned       PC:32;
    unsigned       rd:NR_BITS;
    unsigned       data[NUM_THREADS];
    unsigned       eop:1;
    unsigned       valid:1;


} VX_writeback_if;


typedef struct VX_ibuffer_if
{
    uint64_t     uuid:UUID_BITS;
    unsigned     wid: NW_BITS;
    bool     	 tmask [NUM_THREADS];
    unsigned     PC : 32;
    unsigned     next_PC : 32;
    unsigned     ex_type: EX_BITS;
    unsigned     op_type: INST_OP_BITS;
    unsigned     op_mod: INST_MOD_BITS;
    signed       imm:32;
    unsigned     rd :NR_BITS;
    unsigned     rs1:NR_BITS;
    unsigned     rs2:NR_BITS;
    unsigned     rs3:NR_BITS;
    unsigned     valid:1;
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
void dispatch  (VX_ibuffer_if ibuffer_if,bool* ready_ibuffer, VX_gpr_rsp_if gpr_rsp_if,
			VX_alu_req_if *alu_req_if, bool ready_gpu,
			VX_fpu_req_if *fpu_req_if, bool ready_fpu,
			VX_csr_req_if *csr_req_if, bool ready_csr,
			VX_lsu_req_if *lsu_req_if, bool ready_lsu,
			VX_gpu_req_if *gpu_req_if, bool ready_alu);

