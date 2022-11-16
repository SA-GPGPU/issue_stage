#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

//declaration
bool read_ibuffer;
VX_decode ibuffer;
VX_gpr_rsp gpr_rsp;
VX_gpr_req gpr_req;

void issue_stage(VX_decode decode, VX_writeback writeback,
				 VX_alu_req *alu_req, VX_lsu_req *lsu_req,
				 VX_csr_req *csr_req, VX_fpu_req *fpu_req, VX_gpu_req *gpu_req)
{


    //calling functions
    ibuffer_stage ( decode, &ibuffer,1,0);
    assign_gpr_req( ibuffer,  &gpr_req);
    GPRs ( writeback,  gpr_req,  &gpr_rsp  ) ;
	scoreboard ( ibuffer,  writeback, &read_ibuffer);
    ibuffer_stage ( decode, &ibuffer,0, read_ibuffer);
    dispatch  (ibuffer, &read_ibuffer, gpr_rsp,
               &*alu_req, &*lsu_req, &*csr_req,
               &*fpu_req, &*gpu_req);
    printf_decode( decode);
	printf_ibuffer( ibuffer);
	printf_gpr_rsp( gpr_rsp);
	if  (decode.ex_type==1)  printf_alu_req( *alu_req);
	if  (decode.ex_type==2)  printf_lsu_req( *lsu_req);
	if  (decode.ex_type==3)  printf_csr_req( *csr_req);
	if  (decode.ex_type==4)  printf_fpu_req( *fpu_req);
	if  (decode.ex_type==5)  printf_gpu_req( *gpu_req);


}
