#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"


int main()
{
    VX_decode decoded_signals;
    VX_decode ibuffer;
    VX_writeback writeback;
    VX_alu_req alu_req;
    VX_lsu_req lsu_req;
    VX_csr_req csr_req;
    VX_fpu_req fpu_req;
    VX_gpu_req gpu_req;

    FILE *fptr ; // file pointer
    char input_line [400] ;
    fptr=fopen("./issue_input.txt","r");
    if (fptr==NULL)
	{
    	printf("Error opening input file \n") ;
    	exit(1) ;
    }

    while (fgets (input_line,400,fptr) != NULL)
	{ // getting a line from file
    	char *value [30] ;
    	printf("%s \n",input_line);

        char *token4=strtok(input_line,"{");
        token4=strtok(NULL,"}");
    	if (token4 != NULL)
		{
            token4=strtok(token4,",");
            writeback.data[3]=(int) strtoull(token4,NULL,16);
    		for (int i=0;i<NUM_THREADS-1;i++)
			{
    		token4=strtok(NULL,",");
            writeback.data[2-i]=(int) strtoull(token4,NULL,16);
			}
    	}

    	char *token=strtok(input_line,"=");


    	int i=0 ;
    	while (token != NULL)
		{
    		value[i]=strtok(NULL,",");
    		token=strtok(NULL,"=");
    		i++;
    	}

    	char *token3=strtok(input_line,"(");
    	token3=strtok(NULL,")");


    	if(strcmp(token3, "Decode IF ONLY") == 0)
        {
			decoded_signals.uuid = (long int) strtoull (value[0],NULL,16) ;
			decoded_signals.wid = (int) strtoul(value[1],NULL,16) ;
			decoded_signals.tmask = (int) strtoul(value[2],NULL,16) ;
			decoded_signals.PC = (unsigned) strtoul (value[3],NULL,16) ;
			decoded_signals.ex_type = (int) strtoul(value[4],NULL,16) ;
			decoded_signals.op_type = (int) strtoul(value[5],NULL,16) ;
			decoded_signals.op_mod = (int) strtoul(value[6],NULL,16) ;
			decoded_signals.wb = (int) strtoul(value[7],NULL,16) ;
			decoded_signals.rd = (int) strtoul(value[8],NULL,16) ;
			decoded_signals.rs1 = (int) strtoul(value[9],NULL,16) ;
			decoded_signals.rs2 = (int) strtoul(value[10],NULL,16) ;
			decoded_signals.rs3 = (int) strtoul(value[11],NULL,16) ;
			decoded_signals.imm = (int) strtoul(value[12],NULL,16) ;
			decoded_signals.use_PC = (int) strtoul(value[13],NULL,16) ;
			decoded_signals.use_imm = (int) strtoul(value[14],NULL,16) ;
			writeback.eop=0;
			issue_stage( decoded_signals,  writeback,
						&alu_req,  &lsu_req,
						&csr_req,  &fpu_req, &gpu_req,0);
        }

    	if(strcmp(token3, "Write back ONLY") == 0)
        {
			writeback.uuid=(long int) strtoull(value[0],NULL,16)  ;
			writeback.wid=(int) strtoul(value[1],NULL,16)   ; //equal to decode.wid to read from the same address that we write on
			writeback.tmask=(int) strtoul(value[2],NULL,16); // 0b1111 to write in all threads
			writeback.PC=(int) strtoul(value[3],NULL,16);
			writeback.rd=(int) strtoul(value[4],NULL,16); //equal to decode.rd to read from the same address that we write on
			writeback.eop=(int) strtoul(value[5],NULL,16);
			issue_stage( decoded_signals,  writeback,
						&alu_req,  &lsu_req,
						&csr_req,  &fpu_req, &gpu_req,1);
        }

         if(strcmp(token3, "Decode and WB") == 0)
        {

			decoded_signals.uuid = (long int) strtoull (value[0],NULL,16) ;
			decoded_signals.wid = (int) strtoul(value[1],NULL,16) ;
			decoded_signals.tmask = (int) strtoul(value[2],NULL,16) ;
			decoded_signals.PC = (unsigned) strtoul (value[3],NULL,16) ;
			decoded_signals.ex_type = (int) strtoul(value[4],NULL,16) ;
			decoded_signals.op_type = (int) strtoul(value[5],NULL,16) ;
			decoded_signals.op_mod = (int) strtoul(value[6],NULL,16) ;
			decoded_signals.wb = (int) strtoul(value[7],NULL,16) ;
			decoded_signals.rd = (int) strtoul(value[8],NULL,16) ;
			decoded_signals.rs1 = (int) strtoul(value[9],NULL,16) ;
			decoded_signals.rs2 = (int) strtoul(value[10],NULL,16) ;
			decoded_signals.rs3 = (int) strtoul(value[11],NULL,16) ;
			decoded_signals.imm = (int) strtoul(value[12],NULL,16) ;
			decoded_signals.use_PC = (int) strtoul(value[13],NULL,16) ;
			decoded_signals.use_imm = (int) strtoul(value[14],NULL,16) ;
			writeback.uuid=(long int) strtoull(value[15],NULL,16)  ;
			writeback.wid=(int) strtoul(value[16],NULL,16)   ;
			writeback.tmask=(int) strtoul(value[17],NULL,16);
			writeback.PC=(int) strtoul(value[18],NULL,16);
			writeback.rd=(int) strtoul(value[19],NULL,16);
			writeback.eop=(int) strtoul(value[20],NULL,16);
			issue_stage( decoded_signals,  writeback,
						&alu_req,  &lsu_req,
						&csr_req,  &fpu_req, &gpu_req,0);
        }
    }
    fclose(fptr);
	return 0;
}

