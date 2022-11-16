#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

//declaration
bool 		 alu_req_valid;
unsigned	 alu_op_type;
bool 		 lsu_req_valid;
unsigned	 lsu_op_type;
bool		 lsu_is_fence;
bool		 lsu_is_prefetch;
bool 		 csr_req_valid;
unsigned	 csr_op_type;
unsigned	 csr_addr;
unsigned	 csr_imm;
unsigned	 csr_rs1_data;
bool 		 fpu_req_valid;
unsigned	 fpu_op_type;
bool 		 gpu_req_valid;
unsigned	 gpu_op_type;
unsigned 	 tid;
unsigned     next_PC;

 void dispatch  (VX_decode ibuffer,  bool* ready_ibuffer, VX_gpr_rsp gpr_rsp,
				 VX_alu_req *alu_req, VX_lsu_req *lsu_req, VX_csr_req *csr_req,
				 VX_fpu_req *fpu_req, VX_gpu_req *gpu_req)


 {

	//assign next_PC
	next_PC = ibuffer.PC + 4;

	//assign thread ID
	for(int i = 0; i <NUM_THREADS; i++)
	{
        if(getBit (ibuffer.tmask , i)==1 )
		{
			 tid=i;
			break;
		}
	}

     // assign data_in_alu;
    alu_req_valid = (ibuffer.ex_type == EX_ALU);
    alu_op_type = (ibuffer.op_type);
	if(alu_req_valid)  assign_alu( ibuffer, gpr_rsp, next_PC, alu_op_type, tid, &*alu_req);

     // assign data_in_lsu;
    lsu_req_valid =  (ibuffer.ex_type == EX_LSU);
    lsu_op_type = (ibuffer.op_type);
    lsu_is_fence = (ibuffer.op_mod);
    lsu_is_prefetch = (ibuffer.op_mod);
	if(lsu_req_valid)  assign_lsu( ibuffer, gpr_rsp, lsu_op_type, lsu_is_fence, lsu_is_prefetch, &*lsu_req);

	 // assign data_in_csr
    csr_req_valid = (ibuffer.ex_type == EX_CSR);
    csr_op_type = (ibuffer.op_type);
    csr_addr = getBits(ibuffer.imm,CSR_ADDR_BITS-1,0);
	//imm???
    csr_rs1_data = gpr_rsp.rs1_data[tid];
	if(csr_req_valid)  assign_csr( ibuffer, csr_op_type, csr_addr, csr_imm, csr_rs1_data, &*csr_req);

	// assign data_in_fpu;
    fpu_req_valid =  (ibuffer.ex_type == EX_FPU);
    fpu_op_type = (ibuffer.op_type);
	if(fpu_req_valid)  assign_fpu( ibuffer, gpr_rsp, fpu_op_type, &*fpu_req);


	// assign data_in_gpu;
    gpu_req_valid =(ibuffer.ex_type == EX_GPU);
    gpu_op_type = (ibuffer.op_type);
	if(gpu_req_valid) assign_gpu(ibuffer, gpr_rsp, next_PC, gpu_op_type, tid, &*gpu_req);

 }
