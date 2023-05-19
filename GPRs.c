#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

//declaration
unsigned ram_rs1 [NUM_THREADS][NUM_WARPS*NUM_REGS];
unsigned ram_rs2 [NUM_THREADS][NUM_WARPS*NUM_REGS];
unsigned ram_rs3 [NUM_THREADS][NUM_WARPS*NUM_REGS];
bool wren[NUM_THREADS];
unsigned waddr, raddr1, raddr2,raddr3;



//====================================================================================================================
 void GPRs (VX_writeback writeback, VX_gpr_req gpr_req, VX_gpr_rsp *gpr_rsp  )
 {
    // ensure r0 never gets written, which can happen before the reset
    for ( int i = 0; i <NUM_THREADS; i++) wren[i] = (writeback.rd != 0) && (getBit (writeback.tmask , i)==1);

	//Write address is concatenation between writeback warp ID and RD
	//Read address is concatenation between gpr_req warp ID and RS
	waddr= concatenate ( writeback.wid, NW_BITS, writeback.rd, NR_BITS);
	raddr1= concatenate ( gpr_req.wid, NW_BITS,  gpr_req.rs1, NR_BITS);
	raddr2= concatenate ( gpr_req.wid, NW_BITS,  gpr_req.rs2, NR_BITS);
	raddr3= concatenate ( gpr_req.wid, NW_BITS,  gpr_req.rs3, NR_BITS);

	//There is one ram for each thread to execute multiple threads
	//There are different enables, write and read data for each ram
	//There is one group of rams for each source address
	//There are 3 source addresses and 4 threads so there are 12 rams
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i])
		{
			//Write data is input from writeback data
             ram_rs1[i][waddr] = writeback.data[i];
		}
		//Read data is the output of block to gpr_rsp data
		(gpr_rsp->rs1_data[i])=ram_rs1[i][raddr1];

	}
	//printf("raddr1=%x\n",raddr1);
	//printf("ram_rs1[0][f]=%x\n",ram_rs1[0][14]);
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i])
		{
			//Write data is input from writeback data
             ram_rs2[i][waddr] = writeback.data[i];
		}
		//Read data is the output of block to gpr_rsp data
		(gpr_rsp->rs2_data[i])=ram_rs2[i][raddr2];
	}
    for (int i = 0; i <NUM_THREADS; i++) {
		if(wren[i])
		{
			//Write data is input from writeback data
             ram_rs3[i][waddr] = writeback.data[i];
		}
		//Read data is the output of block to gpr_rsp data
		(gpr_rsp->rs3_data[i])=ram_rs3[i][raddr3];

	}


 }

