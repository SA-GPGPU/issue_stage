
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
				 VX_csr_req *csr_req, VX_fpu_req *fpu_req, VX_gpu_req *gpu_req,bool wb)
{
    //calling functions
    if(wb)
    {
    decode.wb=0;
	scoreboard ( decode,  writeback, &read_ibuffer);
    ibuffer_stage ( decode, &ibuffer,0, 1);
    assign_gpr_req( ibuffer,  &gpr_req);
    GPRs ( writeback,  gpr_req,  &gpr_rsp  ) ;
    dispatch  (ibuffer, gpr_rsp,
               &*alu_req, &*lsu_req, &*csr_req,
               &*fpu_req, &*gpu_req);
    }
    else
    {
    ibuffer_stage ( decode, &ibuffer,1,0);
	scoreboard ( decode,  writeback, &read_ibuffer);
    ibuffer_stage ( decode, &ibuffer,0, read_ibuffer);
    assign_gpr_req( ibuffer,  &gpr_req);
    GPRs ( writeback,  gpr_req,  &gpr_rsp  ) ;
    dispatch  (ibuffer, gpr_rsp,
               &*alu_req, &*lsu_req, &*csr_req,
               &*fpu_req, &*gpu_req);
    }

}
