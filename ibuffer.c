/*algorithm
The output of the Ibuffer is instruction that has 3 probabilities
1-The read instruction from ibuffer when read signal=1
2-The input instruction from decode stage when
    The number of added warps=0 and we need to write
    The number of added warps=0 and we need to read recently enqueued instruction
    There is added warp but it is empty and we need to write
3-The same saved output when
    The warp we need to write in isn’t empty
    The number of added warps=0 and we need to read when there isn’t any data
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"
//Declaration
bool        writing[NUM_WARPS];
bool        reading[NUM_WARPS];
bool        warp_added;
bool        warp_removed;
unsigned  	used_r[NUM_WARPS];
//We need to sure that the buffer is not full to write in it
//We need to know that the buffer is empty or not because
//if it is empty we can pass the written instruction to the next stage directly
//So we need used registers table which represents each instruction location by bit
//that set if we wrote in it and reset if we read from it
bool  		full_r[NUM_WARPS];
bool 		not_empty_r[NUM_WARPS];
bool 		not_alm_empty_r[NUM_WARPS];
unsigned  		added_warps;
unsigned 		deq_wid;
bool  		valid_table[NUM_WARPS];
//We need to know which warp will execute firstly
//The used warp scheduling algorithm is round-robin.
//So we need valid table which represents each warp by bit
//that set if we wrote in it and reset if we read from it
VX_decode buffer[NUM_WARPS][IBUF_SIZE];
//Instruction buffer has  buffer for each warp as there are 4 warps
// buffer can store 2 instructions



//====================================================================================================================
void ibuffer_stage (VX_decode decode,VX_decode *ibuffer,bool write_ibuffer,bool read_ibuffer)
{

	if (write_ibuffer) valid_table[decode.wid] = 1;
    deq_wid=decode.wid;
	if (read_ibuffer) valid_table[deq_wid] = not_alm_empty_r[deq_wid];

    for ( int i = 0; i <NUM_WARPS; i++)
	{
	    writing[i] = write_ibuffer &&  (i == decode.wid);
	    reading[i] = read_ibuffer &&  (i == deq_wid);
		if(writing[i])
		{
			if (added_warps >= 1 && !(reading[i] && !not_alm_empty_r[deq_wid]))
			{
				if(!full_r[decode.wid])
				{
					if(!not_empty_r[decode.wid])
					{
						assign_ibuffer(decode, &*ibuffer);
						assign_ibuffer(decode, &buffer[decode.wid][used_r[decode.wid]]);


					}
					else
					{
						assign_ibuffer(decode, &buffer[decode.wid][used_r[decode.wid]]);

					}

				}
			}
			else
            {
						assign_ibuffer(decode, &buffer[decode.wid][used_r[decode.wid]]);
                        assign_ibuffer(decode, &*ibuffer);

            }
		}

		if(reading[i])
		{
			if (added_warps >= 1)
			{
                assign_ibuffer(buffer[deq_wid][0], &*ibuffer);
         if((used_r[deq_wid])==2)
       assign_ibuffer(buffer[deq_wid][(used_r[deq_wid])-1], &buffer[deq_wid][(used_r[deq_wid])-2]);
			}
			else if(writing[i]) assign_ibuffer(decode, &*ibuffer);
		}

	}
    warp_removed = read_ibuffer &&
                   !(write_ibuffer && decode.wid == deq_wid) &&
                   !not_alm_empty_r[deq_wid];
     if (warp_removed && !warp_added && added_warps!=0)   added_warps = added_warps - 1;

    warp_added   = write_ibuffer && !not_empty_r[decode.wid];
    if (warp_added && !warp_removed && added_warps!=NUM_WARPS) 		added_warps = added_warps + 1;
    for ( int i = 0; i <NUM_WARPS; i++)
    {

	    writing[i] = write_ibuffer &&  (i == decode.wid);
	    reading[i] = read_ibuffer &&  (i == deq_wid);
        if (reading[i] && !writing[i])
		{
		 full_r[i] = 0;
		 if (used_r[i] == 0) not_empty_r[i] = 0;
		 if (used_r[i] == 1)  not_alm_empty_r[i] = 0;
		}
        if(!full_r[i] && !(reading[i]&&!not_empty_r[i]))used_r[i] = used_r[i] + writing[i] - reading[i];
		if (writing[i] && !reading[i])
		{
		 not_empty_r[i] = 1;
		 if (used_r[i] == 1) not_alm_empty_r[i] = 1;
		 if (used_r[i] == IBUF_SIZE)  full_r[i] = 1;
		}
	}

}

