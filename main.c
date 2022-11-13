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
    bool ready_ibuffer, ready_decode;
    bool initial_flag;
    ibuffer ( decode_if, ready_ibuffer, &ibuffer_if,  &ready_decode,initial_flag);
    scoreboard ( ibuffer_if,  writeback_if, &ready_ibuffer,initial_flag);
    gpr_stage ( writeback_if,  gpr_req_if,  &gpr_rsp_if  ) ;

    return 0;
}
