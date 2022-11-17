#include <stdio.h>
#include <stdint.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "decoder.h"

#define DATAW   UUID_BITS + NUM_THREADS + 32 + EX_BITS + INST_OP_BITS + INST_FRM_BITS + 1 + (NR_BITS * 4) + 32 + 1 + 1
#define ADDRW    int_log2(IBUF_SIZE+1)		 // round up log2
#define NWARPSW  int_log2(NUM_WARPS+1)

#define DATAW_alu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (2 * NUM_THREADS * 32)
#define DATAW_lsu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32) + 1
#define DATAW_csr   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS + 32
#define DATAW_fpu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (3 * NUM_THREADS * 32)
#define DATAW_gpu   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (3 * NUM_THREADS * 32)


typedef struct VX_writeback
{
    uint64_t       uuid:UUID_BITS;
    unsigned       wid:NW_BITS;
    unsigned       tmask:NUM_THREADS;
    unsigned       PC:32;
    unsigned       rd:NR_BITS;
    unsigned       data[NUM_THREADS];
    unsigned       eop:1;


} VX_writeback;

typedef struct VX_gpr_req
{
    unsigned       wid:NW_BITS;
    unsigned       rs1:NR_BITS;
    unsigned       rs2:NR_BITS;
    unsigned       rs3:NR_BITS;


} VX_gpr_req;

typedef struct VX_gpr_rsp
{
    unsigned       rs1_data[NUM_THREADS];
    unsigned       rs2_data[NUM_THREADS];
    unsigned       rs3_data[NUM_THREADS];
} VX_gpr_rsp;

typedef struct VX_alu_req
{
    uint64_t    	uuid:UUID_BITS;
    unsigned    	wid:NW_BITS;
    unsigned  	 	tmask:NUM_THREADS;
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


} VX_alu_req;

typedef struct VX_lsu_req
{
    uint64_t    	uuid:UUID_BITS;
    bool            is_fence;
    bool            is_prefetch;
    unsigned    	wid:NW_BITS;
    unsigned     	tmask:NUM_THREADS;
    unsigned        PC:32;
    unsigned        offset:32;
    unsigned 		op_type:INST_LSU_BITS;
    unsigned       	store_data[NUM_THREADS];
    unsigned       	base_addr[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_lsu_req;

typedef struct VX_csr_req
{
    uint64_t    	uuid:UUID_BITS;
    unsigned    	wid:NW_BITS;
    unsigned  	 	tmask:NUM_THREADS;
    unsigned        PC:32;
    unsigned 		op_type:INST_CSR_BITS;
    unsigned		addr:CSR_ADDR_BITS;
    bool            use_imm;
    signed		    imm:32;
    unsigned       	rs1_data;
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_csr_req;


typedef struct VX_fpu_req
{
    uint64_t    	uuid:UUID_BITS;
    unsigned    	wid:NW_BITS;
    unsigned  		tmask:NUM_THREADS;
    unsigned        PC:32;
    unsigned 		op_type:INST_FPU_BITS;
    unsigned		op_mod:INST_MOD_BITS;
    unsigned       	rs1_data[NUM_THREADS];
    unsigned       	rs2_data[NUM_THREADS];
    unsigned       	rs3_data[NUM_THREADS];
    unsigned     	rd:NR_BITS;
    bool            wb;


} VX_fpu_req;

typedef struct VX_gpu_req
{
    uint64_t    	uuid:UUID_BITS;
    unsigned    	wid:NW_BITS;
    unsigned  		tmask:NUM_THREADS;
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


} VX_gpu_req;

//============================= FUNCTIONS ============================================

void issue_stage(VX_decode decode, VX_writeback writeback,
				 VX_alu_req *alu_req, VX_lsu_req *lsu_req,
				 VX_csr_req *csr_req, VX_fpu_req *fpu_req, VX_gpu_req *gpu_req);

void ibuffer_stage (VX_decode decode,VX_decode *ibuffer,bool write_ibuffer,bool read_ibuffer) ;
void gpr_stage (VX_writeback VX_writeback, VX_gpr_req gpr_req, VX_gpr_rsp *gpr_rsp  ) ;
void scoreboard (VX_decode ibuffer, VX_writeback writeback, bool *read_ibuffer  ) ;
 void dispatch  (VX_decode ibuffer,  bool* read_ibuffer, VX_gpr_rsp gpr_rsp,
				 VX_alu_req *alu_req, VX_lsu_req *lsu_req, VX_csr_req *csr_req,
				 VX_fpu_req *fpu_req, VX_gpu_req *gpu_req);
//assign_struct
void assign_gpr_req(VX_decode ibuffer, VX_gpr_req *gpr_req);
void assign_ibuffer(VX_decode decode, VX_decode *ibuffer);
void assign_alu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp, unsigned next_PC,
				unsigned alu_op_type, unsigned tid, VX_alu_req* alu_req);
void assign_lsu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp,unsigned lsu_op_type,
			    bool lsu_is_fence, bool lsu_is_prefetch, VX_lsu_req* lsu_req);
void assign_csr(VX_decode ibuffer, unsigned csr_op_type, unsigned csr_addr,
			     unsigned csr_imm, unsigned csr_rs1_data, VX_csr_req* csr_req);
void assign_fpu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp,
				unsigned fpu_op_type, VX_fpu_req* fpu_req);
void assign_gpu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp, unsigned next_PC,
				unsigned gpu_op_type, unsigned tid, VX_gpu_req* gpu_req);
//print_struct
void printf_alu_req(VX_alu_req alu_req);
void printf_lsu_req(VX_lsu_req lsu_req);
void printf_csr_req(VX_csr_req csr_req);
void printf_fpu_req(VX_fpu_req fpu_req);
void printf_gpu_req(VX_gpu_req gpu_req);
void printf_ibuffer(VX_decode ibuffer);
void printf_decode(VX_decode decode);
void printf_writeback(VX_writeback writeback);
void printf_gpr_rsp(VX_gpr_rsp gpr_rsp);
void printf_gpr_req(VX_gpr_req gpr_req);
