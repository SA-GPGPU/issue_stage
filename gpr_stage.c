#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "define.h"
#include "issue.h"



//====================================================================================================================
 gpr_stage (VX_writeback_if writeback_if, VX_gpr_req_if gpr_req_if, VX_gpr_rsp_if *gpr_rsp_if  )
 {
    // ensure r0 never gets written, which can happen before the reset
	//Write enable is and gate between writeback valid and thread mask
    bool write_enable = writeback_if.valid && (writeback_if.rd != 0);
	bool wren[NUM_THREADS];
    for ( int i = 0; i <NUM_THREADS; i++) wren[i] = write_enable && writeback_if.tmask[i];

	//Write address is concatenation between writeback warp ID and RD
	//Read address is concatenation between gpr_req warp ID and RS
	unsigned waddr, raddr1, raddr2,raddr3;
	waddr= concatenate ( writeback_if.wid, NW_BITS, writeback_if.rd, NR_BITS);
	raddr1= concatenate ( gpr_req_if.wid, NW_BITS,  gpr_req_if.rs1, NR_BITS);
	raddr2= concatenate ( gpr_req_if.wid, NW_BITS,  gpr_req_if.rs2, NR_BITS);
	raddr3= concatenate ( gpr_req_if.wid, NW_BITS,  gpr_req_if.rs3, NR_BITS);

	//There is one ram for each thread to execute multiple threads
	//There are different enables, write and read data for each ram
	//There is one group of rams for each source address
	//There are 3 source addresses and 4 threads so there are 12 rams
	unsigned ram_rs1 [NUM_THREADS][NUM_WARPS*NUM_REGS];
	unsigned ram_rs2 [NUM_THREADS][NUM_WARPS*NUM_REGS];
	unsigned ram_rs3 [NUM_THREADS][NUM_WARPS*NUM_REGS];
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i]=1)
		{
			//Write data is input from writeback data
             ram_rs1[i][waddr] = writeback_if.data;
		}
		//Read data is the output of block to gpr_rsp data
		*(gpr_rsp_if->rs1_data+i)=ram_rs1[i][raddr1];
	}
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i]=1)
		{
			//Write data is input from writeback data
             ram_rs2[i][waddr] = writeback_if.data;
		}
		//Read data is the output of block to gpr_rsp data
		*(gpr_rsp_if->rs1_data+i)=ram_rs2[i][raddr1];
	}
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i]=1)
		{
			//Write data is input from writeback data
             ram_rs3[i][waddr] = writeback_if.data;
		}
		//Read data is the output of block to gpr_rsp data
		*(gpr_rsp_if->rs3_data+i)=ram_rs3[i][raddr1];
	}


 }

