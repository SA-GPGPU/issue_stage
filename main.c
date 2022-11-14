#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue.h"

int main()
{
    VX_decode_if decode_if;
    VX_ibuffer_if ibuffer_if;
    VX_writeback_if writeback_if;
    VX_gpr_req_if gpr_req_if;
    VX_gpr_rsp_if gpr_rsp_if;
    VX_alu_req_if alu_req_if;
    VX_lsu_req_if lsu_req_if;
    VX_gpu_req_if gpu_req_if;
    VX_fpu_req_if fpu_req_if;
    VX_csr_req_if csr_req_if;
    bool ready_ibuffer, ready_decode;
    bool ready_gpu,ready_fpu,ready_csr,ready_lsu,ready_alu;
    bool initial_flag;
    ibuffer ( decode_if, ready_ibuffer, &ibuffer_if,  &ready_decode,initial_flag);
    scoreboard ( ibuffer_if,  writeback_if, &ready_ibuffer,initial_flag);
    gpr_stage ( writeback_if,  gpr_req_if,  &gpr_rsp_if  ) ;
    dispatch  ( ibuffer_if,& ready_ibuffer,  gpr_rsp_if,
			    &alu_req_if, ready_gpu,
                &fpu_req_if, ready_fpu,
                &csr_req_if, ready_csr,
                &lsu_req_if, ready_lsu,
                &gpu_req_if, ready_alu);

    return 0;
}
