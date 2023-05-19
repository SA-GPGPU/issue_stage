#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

void assign_gpr_req(VX_decode ibuffer, VX_gpr_req *gpr_req)
{
    gpr_req->wid = ibuffer.wid;
    gpr_req->rs1 = ibuffer.rs1;
    gpr_req->rs2 = ibuffer.rs2;
    gpr_req->rs3 = ibuffer.rs3;

}

void assign_ibuffer(VX_decode decode, VX_decode *ibuffer)
{
    ibuffer->uuid	= decode.uuid	;
    ibuffer->tmask 	= decode.tmask   ;
    ibuffer->wid     = decode.wid     ;
    ibuffer->PC      = decode.PC      ;
    ibuffer->ex_type = decode.ex_type ;
    ibuffer->op_type = decode.op_type ;
    ibuffer->op_mod  = decode.op_mod  ;
    ibuffer->imm     = decode.imm     ;
    ibuffer->rd      = decode.rd      ;
    ibuffer->rs1     = decode.rs1     ;
    ibuffer->rs2     = decode.rs2     ;
    ibuffer->rs3     = decode.rs3     ;
    ibuffer->wb      = decode.wb      ;
    ibuffer->use_PC  = decode.use_PC  ;
    ibuffer->use_imm = decode.use_imm ;


}

void assign_alu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp, unsigned next_PC,
				unsigned alu_op_type, unsigned tid, VX_alu_req* alu_req)
{
    alu_req->uuid	 = ibuffer.uuid	  	;
    alu_req->wid      = ibuffer.wid      	;
    alu_req->tmask 	 = ibuffer.tmask    	;
    alu_req->PC       = ibuffer.PC       	;
    alu_req->next_PC  = next_PC      	    ;
    alu_req->op_type  = alu_op_type 		  	;
    alu_req->op_mod   = ibuffer.op_mod   	;
    alu_req->imm      = ibuffer.imm      	;
    alu_req->use_PC   = ibuffer.use_PC   	;
    alu_req->use_imm  = ibuffer.use_imm  	;
    alu_req->rd       = ibuffer.rd       	;
    alu_req->wb       = ibuffer.wb       	;
    alu_req->tid      = tid      		   	;
	for(int i = 0; i <NUM_THREADS; i++)
	alu_req->rs1_data[i] = gpr_rsp.rs1_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	alu_req->rs2_data[i] = gpr_rsp.rs2_data[i] 	;

}

void assign_lsu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp,unsigned lsu_op_type,
			    bool lsu_is_fence, bool lsu_is_prefetch, VX_lsu_req* lsu_req)
{
    lsu_req->uuid	 	= ibuffer.uuid	 ;
    lsu_req->wid         = ibuffer.wid     ;
    lsu_req->tmask 	    = ibuffer.tmask	 ;
    lsu_req->PC          = ibuffer.PC		 ;
    lsu_req->op_type     = lsu_op_type		 ;
    lsu_req->is_fence    = lsu_is_fence		 ;
    lsu_req->offset	    = ibuffer.imm 	 ;
    lsu_req->rd          = ibuffer.rd		 ;
    lsu_req->wb          = ibuffer.wb		 ;
	for(int i = 0; i <NUM_THREADS; i++)
	lsu_req->base_addr[i] = gpr_rsp.rs1_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	lsu_req->store_data[i] = gpr_rsp.rs2_data[i] 	;
    lsu_req->is_prefetch = lsu_is_prefetch     ;

}
void assign_csr(VX_decode ibuffer, unsigned csr_op_type, unsigned csr_addr,
			     unsigned csr_imm, unsigned csr_rs1_data, VX_csr_req* csr_req)
{
	csr_req->uuid	 = ibuffer.uuid     	  ;
	csr_req->wid      = ibuffer.wid      	  ;
	csr_req->tmask 	 = ibuffer.tmask    	  ;
	csr_req->PC       = ibuffer.PC       	  ;
	csr_req->op_type  = csr_op_type         	  ;
	csr_req->addr     = csr_addr            	  ;
	csr_req->rd       = ibuffer.rd       	  ;
	csr_req->wb       = ibuffer.wb       	  ;
	csr_req->use_imm  = ibuffer.use_imm  	  ;
	csr_req->imm      = csr_imm             	  ;
	csr_req->rs1_data = csr_rs1_data		  	  ;


}
void assign_fpu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp,
				unsigned fpu_op_type, VX_fpu_req* fpu_req)
{
	fpu_req->uuid	 	= ibuffer.uuid    	  ;
	fpu_req->wid     	= ibuffer.wid     	  ;
	fpu_req->tmask 		= ibuffer.tmask   	  ;
	fpu_req->PC      	= ibuffer.PC      	  ;
	fpu_req->op_type 	= fpu_op_type        	  ;
	fpu_req->op_mod   	= ibuffer.op_mod  	  ;
	fpu_req->rd      	= ibuffer.rd      	  ;
	fpu_req->wb      	= ibuffer.wb      	  ;
	for(int i = 0; i <NUM_THREADS; i++)
	fpu_req->rs1_data[i] = gpr_rsp.rs1_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	fpu_req->rs2_data[i] = gpr_rsp.rs2_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	fpu_req->rs3_data[i] = gpr_rsp.rs3_data[i] 	;

}
void assign_gpu(VX_decode ibuffer,VX_gpr_rsp gpr_rsp, unsigned next_PC,
				unsigned gpu_op_type, unsigned tid, VX_gpu_req* gpu_req)
{
	gpu_req->uuid	 = ibuffer.uuid     	  ;
	gpu_req->wid      = ibuffer.wid      	  ;
	gpu_req->tmask 	 = ibuffer.tmask    	  ;
	gpu_req->PC       = ibuffer.PC       	  ;
	gpu_req->next_PC  = next_PC             	  ;
	gpu_req->op_type  = gpu_op_type         	  ;
	gpu_req->op_mod   = ibuffer.op_mod   	  ;
	gpu_req->rd       = ibuffer.rd       	  ;
	gpu_req->wb       = ibuffer.wb       	  ;
	gpu_req->tid      = tid                 	  ;
	for(int i = 0; i <NUM_THREADS; i++)
	gpu_req->rs1_data[i] = gpr_rsp.rs1_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	gpu_req->rs2_data[i] = gpr_rsp.rs2_data[i] 	;
	for(int i = 0; i <NUM_THREADS; i++)
	gpu_req->rs3_data[i] = gpr_rsp.rs3_data[i] 	;

}
