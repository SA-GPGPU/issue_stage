#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

//declaration
unsigned	 alu_op_type;
unsigned	 lsu_op_type;
bool		 lsu_is_fence;
bool		 lsu_is_prefetch;
unsigned	 csr_op_type;
unsigned	 csr_addr;
unsigned	 csr_imm;
unsigned	 csr_rs1_data;
unsigned	 fpu_op_type;
unsigned	 gpu_op_type;
unsigned 	 tid;
unsigned     next_PC;

 void dispatch  (VX_decode ibuffer,  VX_gpr_rsp gpr_rsp,
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
    alu_req->valid = (ibuffer.ex_type == EX_ALU) &&((ibuffer.uuid!=alu_req->uuid));
    alu_op_type = (ibuffer.op_type);
	if(alu_req->valid)  assign_alu( ibuffer, gpr_rsp, next_PC, alu_op_type, tid, &*alu_req);

     // assign data_in_lsu;
    lsu_req->valid =  (ibuffer.ex_type == EX_LSU)&&(ibuffer.uuid!=lsu_req->uuid);
    lsu_op_type = (ibuffer.op_type);
    lsu_is_fence = (ibuffer.op_mod);
    lsu_is_prefetch = (ibuffer.op_mod);
	if(lsu_req->valid)  assign_lsu( ibuffer, gpr_rsp, lsu_op_type, lsu_is_fence, lsu_is_prefetch, &*lsu_req);

	 // assign data_in_csr
    csr_req->valid = (ibuffer.ex_type == EX_CSR)&&(ibuffer.uuid!=csr_req->uuid);
    csr_op_type = (ibuffer.op_type);
    csr_addr = getBits(ibuffer.imm,CSR_ADDR_BITS-1,0);
	//imm???
	 csr_imm=getBits (ibuffer.imm , CSR_ADDR_BITS+1,NRI_BITS) ;

    csr_rs1_data = gpr_rsp.rs1_data[tid];
	if(csr_req->valid)  assign_csr( ibuffer, csr_op_type, csr_addr, csr_imm, csr_rs1_data, &*csr_req);

	// assign data_in_fpu;
    fpu_req->valid =  (ibuffer.ex_type == EX_FPU)&&(ibuffer.uuid!=fpu_req->uuid);
    fpu_op_type = (ibuffer.op_type);
	if(fpu_req->valid)  assign_fpu( ibuffer, gpr_rsp, fpu_op_type, &*fpu_req);


	// assign data_in_gpu;
    gpu_req->valid =(ibuffer.ex_type == EX_GPU)&&(ibuffer.uuid!=gpu_req->uuid);
    gpu_op_type = (ibuffer.op_type);
	if(gpu_req->valid) assign_gpu(ibuffer, gpr_rsp, next_PC, gpu_op_type, tid, &*gpu_req);
    
    if  (ibuffer.ex_type==1 && alu_req->valid)
    {
        printf("wid=%0x, PC=%0x ex=ALU, tmask=%0x, rd=%0x, rs1_data={%0x,%0x,%0x,%0x}, rs2_data={%0x,%0x,%0x,%0x}\n",
        alu_req->wid,alu_req->PC,alu_req->tmask,alu_req->rd,
        alu_req->rs1_data[3],alu_req->rs1_data[2],alu_req->rs1_data[1],alu_req->rs1_data[0],
        alu_req->rs2_data[3],alu_req->rs2_data[2],alu_req->rs2_data[1],alu_req->rs2_data[0]);
    }
    if  (ibuffer.ex_type==2 && lsu_req->valid)
    {
        printf("wid=%0x , PC=%0x ex=LSU, tmask=%0x ,rd=%0x, offset=%0x, addr={%0x,%0x,%0x,%0x}, data={%0x,%0x,%0x,%0x}\n",
        lsu_req->wid,lsu_req->PC,lsu_req->tmask,lsu_req->rd,lsu_req->offset,
        lsu_req->base_addr[3],lsu_req->base_addr[2],lsu_req->base_addr[1],lsu_req->base_addr[0],
        lsu_req->store_data[3],lsu_req->store_data[2],lsu_req->store_data[1],lsu_req->store_data[0]);
    }
    if  (ibuffer.ex_type==3 && csr_req->valid)
    {
        printf("wid=%0x , PC=%0x, ex=CSR, tmask=%0x ,rd=%0x, addr=%0x, rs1_data=%0x\n",
        csr_req->wid,csr_req->PC,csr_req->tmask,csr_req->rd,csr_req->addr,
        csr_req->rs1_data);
    }
    if  (ibuffer.ex_type==4 && fpu_req->valid)
    {
        printf("wid=%0x , PC=%0x ex=FPU, tmask=%0x ,rd= %0x, rs1_data={%0x,%0x,%0x,%0x}, rs2_data={%0x,%0x,%0x,%0x}, rs3_data={%0x,%0x,%0x,%0x}\n",
        fpu_req->wid,fpu_req->PC,fpu_req->tmask,fpu_req->rd,
        fpu_req->rs1_data[3],fpu_req->rs1_data[2],fpu_req->rs1_data[1],fpu_req->rs1_data[0],
        fpu_req->rs2_data[3],fpu_req->rs2_data[2],fpu_req->rs2_data[1],fpu_req->rs2_data[0],
        fpu_req->rs3_data[3],fpu_req->rs3_data[2],fpu_req->rs3_data[1],fpu_req->rs3_data[0]);
    }
    if  (ibuffer.ex_type==5 && gpu_req->valid)
    {
        printf("wid=%0x , PC=%0x ex=GPU, tmask=%0x ,rd= %0x, rs1_data={%0x,%0x,%0x,%0x}, rs2_data={%0x,%0x,%0x,%0x}, rs3_data={%0x,%0x,%0x,%0x}\n",
        gpu_req->wid,gpu_req->PC,gpu_req->tmask,gpu_req->rd,
        gpu_req->rs1_data[3],gpu_req->rs1_data[2],gpu_req->rs1_data[1],gpu_req->rs1_data[0],
        gpu_req->rs2_data[3],gpu_req->rs2_data[2],gpu_req->rs2_data[1],gpu_req->rs2_data[0],
        gpu_req->rs3_data[3],gpu_req->rs3_data[2],gpu_req->rs3_data[1],gpu_req->rs3_data[0]);
    }
 }
