#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue.h"




//====================================================================================================================
 ibuffer (VX_decode_if decode_if,bool ready_ibuffer,VX_ibuffer_if *ibuffer_if, bool *ready_decode, bool initial_flag)
{

    //Declaration of all variable
    bool   q_data_in[DATAW];
    bool        deq_instr[DATAW];
	char  		used_r[NUM_WARPS];
	char  		added_warps;
	char 		deq_wid;
	char 		deq_wid_rr;
    bool  		full_r[NUM_WARPS];
	bool 		empty_r[NUM_WARPS];
	bool 		alm_empty_r[NUM_WARPS];
    bool        deq_valid;
	bool		ready_in[NUM_WARPS];
	bool		valid_out[NUM_WARPS];
	bool  		valid_table[NUM_WARPS];
    bool q_data_prev[NUM_WARPS][DATAW];
	bool q_data_out[NUM_WARPS][DATAW];
    unsigned enq_fire;
    unsigned deq_fire;
	bool buffer[NUM_WARPS][DATAW];
	bool writing[NUM_WARPS];
	bool reading[NUM_WARPS];
	bool going_empty[NUM_WARPS];
	bool pop[NUM_WARPS];
	bool push[NUM_WARPS];
    bool warp_added;
    bool warp_removed;


     // assign data input from decode stage;
     for(int j = 0; j <UUID_BITS; j++) q_data_in[j] = getBit (decode_if.uuid,j) ;

     for(int j = UUID_BITS; j <UUID_BITS+NUM_THREADS; j++)
		 q_data_in[j] = getBit (decode_if.tmask,j-UUID_BITS) ;

     for(int j = UUID_BITS+NUM_THREADS; j <UUID_BITS+NUM_THREADS+32; j++)
		 q_data_in[j] = getBit (decode_if.PC,j-(UUID_BITS+NUM_THREADS)) ;

     for(int j = UUID_BITS+NUM_THREADS+32; j <UUID_BITS+NUM_THREADS+32+EX_BITS; j++)
        q_data_in[j] = getBit (decode_if.ex_type,j-(UUID_BITS+NUM_THREADS+32)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS; j <UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS; j++)
      q_data_in[j] = getBit (decode_if.op_type,j-(UUID_BITS+NUM_THREADS+32+EX_BITS)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS;
     j <UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS; j++)
      q_data_in[j] = getBit (decode_if.op_mod,j-(UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS;
      j <UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+1; j++)
       q_data_in[j] = getBit (decode_if.wb,j-(UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+1;
      j <UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+2; j++)
      q_data_in[j] = getBit (decode_if.use_PC,j-(UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+1)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+2;
      j <UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3; j++)
       q_data_in[j] = getBit (decode_if.use_imm,j-(UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+2)) ;

     for(int j = UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3;
      j <UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3; j++)
      q_data_in[j] = getBit (decode_if.imm,j-(UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3)) ;

     for(int j = UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3;
     j <UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS; j++)
     q_data_in[j] = getBit (decode_if.rd,j-(UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3)) ;

     for(int j = UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS;
      j <UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*2; j++)
       q_data_in[j] = getBit (decode_if.rs1,j-(UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS)) ;

     for(int j = UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*2;
      j <UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*3; j++)
      q_data_in[j] = getBit (decode_if.rs2,j-(UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*2)) ;

     for(int j = UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*2; j <DATAW; j++)
        q_data_in[j] = getBit (decode_if.rs3,j-(UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS*3)) ;


	//initialization of signals
	if(initial_flag ==1)
	{
	//We need to sure that the buffer is not full to write in it
	//We need to know that the buffer is empty or not because if it is empty
	//so we can pass the written instruction to the next stage directly
	//So we need used registers table which represents each instruction location
	//by bit that set if we wrote in it and reset if we read from it
	for(int j = 0; j <NUM_WARPS; j++) used_r[j]=0;
	for(int j = 0; j <NUM_WARPS; j++) full_r[j]=0;
	for(int j = 0; j <NUM_WARPS; j++) empty_r[j]=1;
	for(int j = 0; j <NUM_WARPS; j++) alm_empty_r[j]=1;

	//we enqueue instruction in ibuffer when decode sends valid signal (decode_if_valid)
	//and the buffer isn't full (decode_if_ready)
	//we dequeue instruction from ibuffer when scoreboard send ready signal (ibuffer_if_ready)
	//and the buffer is valid (ibuffer_if_valid) when added_warps > 1 or when there is enqueued instruction we need to dequeue recently
	ready_decode = ! full_r[decode_if.wid];
	enq_fire = decode_if.valid && (unsigned)ready_decode;		// enqueue
	deq_valid = enq_fire;
    ibuffer_if->valid =  deq_valid;
    deq_fire = ibuffer_if->valid&& (unsigned)ready_ibuffer;	// dequeue


    //skid_buffer
	for(int j = 0; j <NUM_WARPS; j++)  ready_in[j]=1;
	for(int j = 0; j <NUM_WARPS; j++)  valid_out[j]=0;

	//We need to know which warp will execute firstly
	//The used warp scheduling algorithm is round-robin.
	//So we need valid table which represents each warp by bit that set if we wrote in it and reset if we read from it
	for(int j = 0; j <NUM_WARPS; j++)  valid_table[j]=1;

    deq_wid_rr=0;
    added_warps=0;
    deq_wid   = decode_if.wid;
	ibuffer_if->wid   =  deq_wid;
    for(int j = 0; j <DATAW; j++)   deq_instr[j] = q_data_in[j];

	}

	//we enqueue instruction in ibuffer when decode sends valid signal (decode_if_valid)
	//and the buffer isn't full (decode_if_ready)
	//we dequeue instruction from ibuffer when scoreboard send ready signal (ibuffer_if_ready)
	//and the buffer is valid (ibuffer_if_valid) when added_warps > 1 or when there is enqueued instruction we need to dequeue recently
    enq_fire = decode_if.valid && (unsigned)ready_decode;		// enqueue
    deq_fire = ibuffer_if->valid&& (unsigned)ready_ibuffer;	// dequeue
    printf("enq_fire= %d\n",enq_fire);
    printf("deq_fire= %d\n",deq_fire);

    for ( int i = 0; i <NUM_WARPS; i++)
    {

		//write and read signals
	    writing[i] = enq_fire && (i == decode_if.wid);
        reading[i] = deq_fire && (i == ibuffer_if->wid);
        printf("writing= %d\n",writing[i]);
        printf("reading= %d\n",reading[i]);

		//We need to sure that the buffer is not full to write in it
		//We need to know that the buffer is empty or not because if it is empty
		//so we can pass the written instruction to the next stage directly
		//So we need used registers table which represents each instruction location
		//by bit that set if we wrote in it and reset if we read from it

		if (writing[i] && !reading[i])
		{
		 empty_r[i] = 0;
		 if (used_r[i] == 1) alm_empty_r[i] = 0;
		 if (used_r[i] == IBUF_SIZE)  full_r[i] = 1;
		}
		else if (reading)
		{
		 full_r[i] = 0;
		 if (used_r[i] == 1) empty_r[i] = 1;
		 if (used_r[i] == 2)  alm_empty_r[i] = 1;
		}
         used_r[i] = used_r[i] + writing[i] - reading[i];

		//we need to know wether the added warp is empty or will be empty to pass the input to output directly
        if (empty_r[i] || (alm_empty_r[i] && reading[i]) ) going_empty[i] = 1;
        else going_empty[i] = 0;
        printf("going_empty= %d\n",going_empty[i]);


        //skid_buffer
		 push[i] = writing[i] && !going_empty[i] && ready_in[i];
		         printf("push= %d\n",push[i]);
        if (!valid_out[i] || reading[i] ) pop[i] = 1;
        else pop[i] = 0;
		if (reading[i]) ready_in[i] = 1;
		else if (writing[i] && !going_empty[i] && valid_out[i]) ready_in[i] = 0;
	    if (pop[i])
		{
			if (writing[i] && !going_empty[i] || !ready_in[i] > 0) valid_out[i] = 1;
			else valid_out[i] = 0;
		}
        if (push[i])
        {
            for(int j = 0; j <DATAW; j++) buffer[i][j] = q_data_in[j];
        }
		if (pop[i] && ready_in[i])
        {
            for(int j = 0; j <DATAW; j++) q_data_prev[i][j] = q_data_in[j];
        }

		else if	 (reading[i])
		{
            for(int j = 0; j <DATAW; j++)  q_data_prev[i][j] = buffer[i][j];
        }

		 //output is equal to input if we write and added warp is empty
		 //output is equal to buffer data if we read
        if (writing[i] && going_empty[i])
        {
            for(int j = 0; j <DATAW; j++)   q_data_out[i][j] = q_data_in[j];
        }

        else if (reading[i])
        {
            for(int j = 0; j <DATAW; j++)   q_data_out[i][j] = q_data_prev[i][j];
        }

	}



	if (deq_fire) valid_table[deq_wid] = !alm_empty_r[deq_wid];
	if (enq_fire) valid_table[decode_if.wid] = 1;
	//rr_arbiter
	if((deq_wid_rr==0 && valid_table[0]==0 &&valid_table[1]==0 &&valid_table[2]==0 &&valid_table[3]==1)
		||(deq_wid_rr==1 &&valid_table[0]==1 &&valid_table[2]==0 &&valid_table[3]==0)
		||(deq_wid_rr==2 &&valid_table[0]==1  &&valid_table[3]==0)
		||(deq_wid_rr==3 &&valid_table[0]==1  )) deq_wid_rr=0;
	else if((deq_wid_rr==1 && valid_table[0]==0 &&valid_table[1]==0 &&valid_table[2]==1 &&valid_table[3]==0)
		||(deq_wid_rr==2 &&valid_table[0]==0 &&valid_table[1]==1 &&valid_table[3]==0)
		||(deq_wid_rr==3 &&valid_table[0]==0  &&valid_table[1]==1)
		||(deq_wid_rr==0 &&valid_table[1]==1  )) deq_wid_rr=1;
	else if((deq_wid_rr==1 && valid_table[0]==0 &&valid_table[1]==1 &&valid_table[2]==0 &&valid_table[3]==0)
		||(deq_wid_rr==3 &&valid_table[2]==1 &&valid_table[1]==0 &&valid_table[0]==0)
		||(deq_wid_rr==0 &&valid_table[2]==1  &&valid_table[1]==0)
		||(deq_wid_rr==1 &&valid_table[2]==1  )) deq_wid_rr=2;
	else deq_wid_rr=3;

    if (added_warps > 1)
	{
		deq_valid = 1;
        deq_wid   = deq_wid_rr;     	//deq_wid_rr = deq_wid_rr_n which comes from rr_arbiter
        for(int j = 0; j <DATAW; j++)   deq_instr[j] = q_data_out[deq_wid_rr][j];
	}
    else if (1 == added_warps && !(deq_fire && alm_empty_r[deq_wid]))
	{
        deq_valid = 1;
         if (deq_fire)
        {
            for(int j = 0; j <DATAW; j++)   deq_instr[j] = q_data_prev[deq_wid][j];
        }
        else
        {
            for(int j = 0; j <DATAW; j++)   deq_instr[j] = q_data_out[deq_wid][j];
        }
	}
     else
	 {
        deq_valid = enq_fire;
        deq_wid   = decode_if.wid;
        for(int j = 0; j <DATAW; j++)   deq_instr[j] = q_data_in[j];

	 }

     warp_added   = enq_fire && empty_r[decode_if.wid];
     warp_removed = deq_fire && !(enq_fire && decode_if.wid == deq_wid) && alm_empty_r[deq_wid];
    if (warp_added && !warp_removed) 		added_warps = added_warps + 1;
    else if (warp_removed && !warp_added)   added_warps = added_warps - 1;


	//assign output values
	ready_decode = !full_r[decode_if.wid];
     ibuffer_if->valid = deq_valid;
     ibuffer_if->wid   = deq_wid;
     ibuffer_if->uuid = getBits (deq_instr , UUID_BITS-1,0) ;
     ibuffer_if->tmask = getBits (deq_instr , UUID_BITS+NUM_THREADS-1,UUID_BITS) ;
     ibuffer_if->PC = getBits (deq_instr , UUID_BITS+NUM_THREADS+32-1,
                                           UUID_BITS+NUM_THREADS) ;

     ibuffer_if->ex_type = getBits (deq_instr , UUID_BITS+NUM_THREADS+32+EX_BITS-1,
                                    UUID_BITS+NUM_THREADS+32) ;

     ibuffer_if->op_type = getBits (deq_instr , UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS-1,
                                    UUID_BITS+NUM_THREADS+32+EX_BITS) ;

     ibuffer_if->op_mod = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS-1,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS) ;

     ibuffer_if->wb = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+1-1,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS) ;

     ibuffer_if->use_PC = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+2-1,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+1) ;

     ibuffer_if->use_imm = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3-1,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+2) ;

     ibuffer_if->imm = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3-1,
                                   UUID_BITS+NUM_THREADS+32+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3) ;

     ibuffer_if->rd = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS-1,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3) ;

     ibuffer_if->rs1 = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+2*NR_BITS-1,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+NR_BITS) ;

     ibuffer_if->rs2 = getBits (deq_instr ,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+3*NR_BITS-1,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+2*NR_BITS) ;

     ibuffer_if->rs3 = getBits (deq_instr ,
                                   DATAW-1,
                                   UUID_BITS+NUM_THREADS+64+EX_BITS+INST_OP_BITS+INST_MOD_BITS+3+3*NR_BITS) ;


}
