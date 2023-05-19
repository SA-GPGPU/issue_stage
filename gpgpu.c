//=====================================================================
//
// gpgpu.c                                              Date: 2022/11/11
//
// Author : Mustafa abdelhamid
//=====================================================================

#include <stdio.h>
#include <stdint.h>


#include "gpgpu.h"
#include "common.h"
#include "config.h"
#include "define.h"

void generate_mask (uint8_t thread_mask , uint32_t *rs1_data ,uint8_t *taken_mask ,uint8_t *nottaken_mask  )
{	//printf("NUMBER OF THREADS = %d \n", NUM_THREADS);
	*taken_mask=  (1<<NUM_THREADS)-1 ;
	*nottaken_mask= (1<<NUM_THREADS)-1 ;
	for (int i=0;i<=NUM_THREADS-1;i++)
	{
		if (rs1_data[i] !=0) // taken branch
			{
				*taken_mask &=  thread_mask ;
				*nottaken_mask &= thread_mask &  ~(1<<i); // clear bit
				//printf("Taken\nGenerated masks : \n taken mask = %d \n nottaken mask= %d \n",*taken_mask,*nottaken_mask);

			}
		else
			{
				*taken_mask &=thread_mask & ~(1<<i); // clear bit
				*nottaken_mask &=thread_mask ;
				//printf("not Taken\nGenerated masks : \n taken mask = %d \n nottaken mask= %d \n",*taken_mask,*nottaken_mask);
			}
	}
}
//=================================================================================================================================
void control_tmask (uint8_t is_pred, uint8_t thread_mask,uint32_t rs1_data_per_thread ,uint8_t taken_mask,uint8_t *valid,uint8_t *thread_mask_out)
{	*valid = 1 ; // as long as it's called the output is valid
	if (is_pred)  *thread_mask_out = ( taken_mask==0) ? thread_mask : taken_mask ;
	else *thread_mask_out= rs1_data_per_thread ;
}
//=================================================================================================================================
void control_wmask (	uint32_t rs1_data_per_thread,uint32_t rs2_data_per_thread ,
			uint8_t *valid,uint8_t *warp_mask_out,uint32_t *pc_out)
{
	*valid=1;
	*pc_out=rs2_data_per_thread;
	*warp_mask_out = (1<< (rs2_data_per_thread+1))-1 ;
}

void control_bar (	uint32_t rs1_data_per_thread,uint32_t rs2_data_per_thread ,
			uint8_t *valid,uint8_t *bar_id,uint8_t *bar_size)
{
	*valid= 1 ;
	*bar_id=rs1_data_per_thread;
	*bar_size=rs2_data_per_thread-1 ;
}
