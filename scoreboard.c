#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue.h"

#define deadlock_time_out 1000


//====================================================================================================================
 scoreboard (VX_ibuffer_if ibuffer_if, VX_writeback_if writeback_if, bool *ready_ibuffer, bool initial_flag  )
{
    //Declaration of all variable
	bool reserve_reg,release_reg;
	bool inuse_regs[NUM_WARPS][NUM_REGS];
	bool deq_inuse_rd, deq_inuse_rs1, deq_inuse_rs2, deq_inuse_rs3;
	int deadlock_ctr;

	//initialization of signals
	if(initial_flag ==1)
	{
		ready_ibuffer=1;
		for(int i = 0; i <NUM_WARPS; i++)
			for(int j = 0; j <NUM_REGS; j++)	inuse_regs[i][j]=0;
		deadlock_ctr=0;
	}


	//we can reserve register when all of these happened together:
	//1-When the ibuffer is valid to out the instruction
	//2-When the instruction will writeback data in register
	//3-When the source and destination register aren't in used
	//we can release register when all of these happened together:
	//1-When the writeback stage is valid to out the instruction
	//2-When the operation has been ended
	reserve_reg = ibuffer_if.valid * (unsigned)ready_ibuffer * ibuffer_if.wb;
    release_reg = writeback_if.valid * writeback_if.eop;

	//In used registers table is table which tell us which registers are in used and which are not.
	//Each bit in the table represents register in specific warp
	//The width of the table is warp numbers (4) and the length is register numbers (64)
	if (reserve_reg == true) {
		inuse_regs[ibuffer_if.wid][ibuffer_if.rd]=1;

	}
	if (release_reg == true) {
		inuse_regs[ibuffer_if.wid][ibuffer_if.rd]=0;

	}

	//scoreboard has the next destination and sources registers
	//It checks whether they are in used or not
	//If they aren’t in used, It sends ready signal to the buffer to deque the next instruction
    deq_inuse_rd  = inuse_regs[ibuffer_if.wid][ibuffer_if.rd];
    deq_inuse_rs1 = inuse_regs[ibuffer_if.wid][ibuffer_if.rs1];
    deq_inuse_rs2 = inuse_regs[ibuffer_if.wid][ibuffer_if.rs2];
    deq_inuse_rs3 = inuse_regs[ibuffer_if.wid][ibuffer_if.rs3];
	if ((deq_inuse_rd || deq_inuse_rs1 || deq_inuse_rs2 || deq_inuse_rs3) == false )
	{
		ready_ibuffer=1;
	}
	else ready_ibuffer=0;


	//If an instruction wants to reserve a used register, the instruction is stalled and the deadlock counter starts to count.
	//If an instruction wants to release an unused register, invalid writeback assertion.
    if (ibuffer_if.valid==true && ready_ibuffer==false)
	{
        deadlock_ctr = deadlock_ctr + 1;
		if(deadlock_ctr>= deadlock_time_out)
		{
			printf("stall error\n");
		}
	}
    if (release_reg==true && inuse_regs[writeback_if.wid][writeback_if.rd]==true)
	{
		printf("invalid writeback register\n");
	}
    if (ibuffer_if.valid==true && ready_ibuffer==true)
	{
              deadlock_ctr = 0;
	}

}


