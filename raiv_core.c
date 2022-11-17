
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "define.h"
#include "config.h"
#include "common.h"
//#include "decoder.h"
#include "issue_stage.h"

int main()
{   int CLK_NUM=43;
    instr_fetch_rsp fetched_instruction;
    perf_decode_if decoder_performance;
    VX_decode decoded_signals;
    wstall_if stalled_warps ;
    join_if join_inst;
    VX_writeback writeback;
    VX_alu_req alu_req;
    VX_lsu_req lsu_req;
    VX_csr_req csr_req;
    VX_fpu_req fpu_req;
    VX_gpu_req gpu_req;


	bool wren[NUM_THREADS];

    //initialization
    writeback.uuid=1  ;
    writeback.wid=0   ; //equal to decode.wid to read from the same address that we write on
    writeback.rd=1    ; //equal to decode.rd to read from the same address that we write on
    writeback.tmask=15; // 0b1111 to write in all threads
    writeback.PC=1    ;
    writeback.eop=1   ;
    for ( int i = 0; i <NUM_THREADS; i++)
    {
        wren[i] =  (getBit (writeback.tmask , i)==1);
        if (wren[i])
        {
            writeback.data[i]=21;
        }
    }

	uint32_t test_inst [55] = {
0x0000a0b7,
0x00014097,
0x01e08093,
0x01e0a093,
0x01e0b093,
0x01e0c093,
0x01e0e093,
0x01e0f093,
0x01e09093,
0x01e0d093,
0x41e0d093,
0x001080b3,
0x401080b3,
0x001090b3,
0x0010a0b3,
0x0010b0b3,
0x0010c0b3,
0x0010d0b3,
0x4010d0b3,
0x0010e0b3,
0x0010f0b3,
0x00408083,
0x00409083,
0x0040a083,
0x0040c083,
0x0040d083,
0x00108223,
0x00109223,
0x0010a223,
0x000000ef,
0x064080e7,
0x00109463,
0x0000006f,
0x00108463,
0x0000006f,
0x0010d463,
0x0000006f,
0x0010c463,
0x0000006f,
0x0010f463,
0x0000006f,
0x0010e463,
0x0000006f
/*
0x021080b3,
0x021090b3,
0x0210a0b3,
0x0210b0b3,
0x0210c0b3,
0x0210d0b3,
0x0210e0b3,
0x0210f0b3
*/
	} ;


    for (int i=0;i<CLK_NUM;i++) {
    	fetched_instruction.instr = test_inst[i] ;
    	decode (fetched_instruction,&decoder_performance,&decoded_signals,&stalled_warps,&join_inst);
        issue_stage( decoded_signals,  writeback,
                    &alu_req,  &lsu_req,
                    &csr_req,  &fpu_req, &gpu_req);

    }

	return 0;
}
