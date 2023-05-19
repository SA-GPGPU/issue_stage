#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

#define deadlock_time_out 1000

//Declaration
unsigned inuse_regs[NUM_WARPS][NUM_REGS];
int deadlock_ctr;
bool reserve_reg,release_reg;
bool deq_inuse_rd, deq_inuse_rs1, deq_inuse_rs2, deq_inuse_rs3;
bool flag;

//====================================================================================================================
void  scoreboard (VX_decode ibuffer, VX_writeback writeback, bool *read_ibuffer )
{
    flag=0;
	//we can release register When the operation has been ended
	if(ibuffer.ex_type==0) return;
    release_reg =  writeback.eop;
    if (release_reg && (inuse_regs[writeback.wid][writeback.rd]==0))
	{
		printf("invalid writeback \n");
	}

	if (release_reg)  inuse_regs[writeback.wid][writeback.rd]--;

	//scoreboard has the next destination and sources registers
	//It checks whether they are in used or not
	//If they aren’t in used, It sends ready signal to the buffer to deque the next instruction
    deq_inuse_rd  = (inuse_regs[ibuffer.wid][ibuffer.rd]>=1);
    deq_inuse_rs1 = (inuse_regs[ibuffer.wid][ibuffer.rs1]>=1);
    deq_inuse_rs2 = (inuse_regs[ibuffer.wid][ibuffer.rs2]>=1);
    deq_inuse_rs3 = (inuse_regs[ibuffer.wid][ibuffer.rs3]>=1);
    for(int i=0;i<pow(2,NR_BITS);i++)
    {
      if  (inuse_regs[ibuffer.wid][i]>=1)
      {
          flag=1;
          break;
      }
    }
    if(deadlock_ctr>=1) *read_ibuffer=!(flag);
    else *read_ibuffer=!(deq_inuse_rd||deq_inuse_rs1||deq_inuse_rs2||deq_inuse_rs3);

	//we can reserve register when all of these happened together:
	//1-When the instruction will writeback data in register
	//2-When the source and destination register aren't in used
	reserve_reg =  ibuffer.wb;

	//In used registers table is table which tell us which registers are in used and which are not.
	//Each bit in the table represents register in specific warp
	//The width of the table is warp numbers (4) and the length is register numbers (64)
	if (reserve_reg ) inuse_regs[ibuffer.wid][ibuffer.rd]++;
	//If an instruction wants to reserve a used register, the instruction is stalled and the deadlock counter starts to count.
	//If an instruction wants to release an unused register, invalid writeback assertion.
    if ( *read_ibuffer||release_reg) deadlock_ctr = 0;
    if ( !*read_ibuffer)
	{
        deadlock_ctr = deadlock_ctr + 1;
		if(deadlock_ctr>= deadlock_time_out)
		{
			printf("stall error\n");
		}
	}
    //printf("read_ibuffer=%d\n",*read_ibuffer);
    //printf("deadlock_ctr=%d\n",deadlock_ctr);
}


