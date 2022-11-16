#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"


int main()
{
    //declaration
    VX_decode decode;
    VX_writeback writeback;
    VX_alu_req alu_req;
    VX_lsu_req lsu_req;
    VX_csr_req csr_req;
    VX_fpu_req fpu_req;
    VX_gpu_req gpu_req;
	bool wren[NUM_THREADS];

    //initialization
    decode.uuid    =1 ;
    decode.tmask   =7 ;
    decode.wid     =1 ;
    decode.PC      =0 ;
    decode.ex_type =1 ; //1 for alu //2 for lsu //3 for csr //4 for fpu //5 for gpu
    decode.op_type =3 ;
    decode.op_mod  =4 ;
    decode.imm 	  =4 ;
    decode.rd 	  =1 ;
    decode.rs1 	  =1 ;
    decode.rs2 	  =1 ;
    decode.rs3 	  =1 ;
    decode.wb 	  =1 ;
    decode.use_PC  =1 ;
    decode.use_imm =1 ;
    writeback.uuid=1  ;
    writeback.wid=1   ; //equal to decode.wid to read from the same address that we write on
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

    //calling function
    issue_stage( decode,  writeback,
                 &alu_req,  &lsu_req,
                 &csr_req,  &fpu_req, &gpu_req);





    return 0;
}
