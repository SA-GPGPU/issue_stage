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
    VX_decode ibuffer;
    VX_decode decoded_signals;
    VX_writeback writeback;
    VX_alu_req alu_req;
    VX_lsu_req lsu_req;
    VX_csr_req csr_req;
    VX_fpu_req fpu_req;
    VX_gpu_req gpu_req;
    alu_req.uuid=20;
    lsu_req.uuid=20;
    csr_req.uuid=20;
    fpu_req.uuid=20;
    gpu_req.uuid=20;

    FILE *fptr ; // file pointer
    FILE *fptr_out ; // file pointer
    char input_line [400] ;
    char input_line_out [400] ;
    fptr=fopen("./issue_input.txt","r");
    fptr_out=fopen("./issue_output.txt","r");
    if (fptr==NULL)
	{
    	printf("Error opening input file \n") ;
    	exit(1) ;
    }

    while (fgets (input_line,400,fptr) != NULL)
	{ // getting a line from file
    	char *value [30] ;
    	//printf("%s \n",input_line);

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
			if  (ibuffer.ex_type==1 && alu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( alu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( alu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}

				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
                {
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
                }
				if ( alu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",alu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( alu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",alu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( alu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",alu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( alu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",alu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==2 && lsu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token6=strtok(input_line_out,"{");
				token6=strtok(NULL,"}");
				if (token6 != NULL)
				{
					token6=strtok(token6,",");
					if ( lsu_req.base_addr[3] != (int) strtoull(token6,NULL,16) )
					{
						printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[3],(int) strtol(token6,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token6=strtok(NULL,",");
						if ( lsu_req.base_addr[2-i] != (int) strtoull(token6,NULL,16) )
						{
							printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[2-i],(int) strtol(token6,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( lsu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",lsu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",lsu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",lsu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",lsu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==3 && csr_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( csr_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",csr_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( csr_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",csr_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( csr_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",csr_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",csr_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
				if ( csr_req.addr != (int) strtol(value[5],NULL,16) )
				{
					printf("FAILED (addr mismatch) \n output= %0x , expected=%0x \n",csr_req.addr,(int) strtol(value[5],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rs1_data != (int) strtol(value[6],NULL,16) )
				{
					printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",csr_req.rs1_data,(int) strtol(value[6],NULL,16)) ;
					return 0;
				}

			}
			if  (ibuffer.ex_type==4 && fpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( fpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",fpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",fpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",fpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",fpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==5 && gpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( gpu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( gpu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( gpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",gpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",gpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",gpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",gpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
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
			if  (ibuffer.ex_type==1 && alu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( alu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( alu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}

				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
                {
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
                }
				if ( alu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",alu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( alu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",alu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( alu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",alu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( alu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",alu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==2 && lsu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				token5=strtok(NULL,"}");
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( lsu_req.base_addr[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( lsu_req.base_addr[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( lsu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",lsu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",lsu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",lsu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",lsu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==3 && csr_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( csr_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",csr_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( csr_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",csr_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( csr_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",csr_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",csr_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
				if ( csr_req.addr != (int) strtol(value[5],NULL,16) )
				{
					printf("FAILED (addr mismatch) \n output= %0x , expected=%0x \n",csr_req.addr,(int) strtol(value[5],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rs1_data != (int) strtol(value[6],NULL,16) )
				{
					printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",csr_req.rs1_data,(int) strtol(value[6],NULL,16)) ;
					return 0;
				}

			}
			if  (ibuffer.ex_type==4 && fpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( fpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",fpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",fpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",fpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",fpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==5 && gpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( gpu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( gpu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( gpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",gpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",gpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",gpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",gpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
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

			if  (ibuffer.ex_type==1 && alu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( alu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( alu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",alu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}

				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
                {
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
                }
				if ( alu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",alu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( alu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",alu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( alu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",alu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( alu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",alu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==2 && lsu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				token5=strtok(NULL,"}");
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( lsu_req.base_addr[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( lsu_req.base_addr[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (base_addr mismatch) \n output= %0x , expected=%0x \n",lsu_req.base_addr[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( lsu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",lsu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",lsu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",lsu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( lsu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",lsu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==3 && csr_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( csr_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",csr_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( csr_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",csr_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( csr_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",csr_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",csr_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
				if ( csr_req.addr != (int) strtol(value[5],NULL,16) )
				{
					printf("FAILED (addr mismatch) \n output= %0x , expected=%0x \n",csr_req.addr,(int) strtol(value[5],NULL,16)) ;
					return 0;
				}
				if ( csr_req.rs1_data != (int) strtol(value[6],NULL,16) )
				{
					printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",csr_req.rs1_data,(int) strtol(value[6],NULL,16)) ;
					return 0;
				}

			}
			if  (ibuffer.ex_type==4 && fpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( fpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",fpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",fpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",fpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( fpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",fpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}
			if  (ibuffer.ex_type==5 && gpu_req.valid)
			{
				fgets (input_line_out,400,fptr_out) ;// get output line
				char *token5=strtok(input_line_out,"{");
				//printf("token5=%s\n",token5);
				token5=strtok(NULL,"}");
				//printf("token5\2=%s\n",token5);
				if (token5 != NULL)
				{
					token5=strtok(token5,",");
					if ( gpu_req.rs1_data[3] != (int) strtoull(token5,NULL,16) )
					{
						printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[3],(int) strtol(token5,NULL,16)) ;
						return 0;
					}
					for (int i=0;i<NUM_THREADS-1;i++)
					{
						token5=strtok(NULL,",");
						if ( gpu_req.rs1_data[2-i] != (int) strtoull(token5,NULL,16) )
						{
							printf("FAILED (rs1_data mismatch) \n output= %0x , expected=%0x \n",gpu_req.rs1_data[2-i],(int) strtol(token5,NULL,16)) ;
							return 0;
						}
					}
				}
				token=strtok(input_line_out,"=");
				int j=0 ;
				while (token != NULL)
				{
					value[j]=strtok(NULL,",");
					token=strtok(NULL,"=");
					j++;
				}
				if ( gpu_req.wid != (int) strtol(value[0],NULL,16) )
				{
					printf("FAILED (wid mismatch) \n output= %0x , expected=%0x \n",gpu_req.wid,(int) strtol(value[0],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.PC != (int) strtoul(value[1],NULL,16) )
				{
					printf("FAILED (PC mismatch) \n output= %0x , expected=%0x \n",gpu_req.PC,(int) strtoul(value[1],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.tmask != (int) strtol(value[3],NULL,16) )
				{
					printf("FAILED (tmask mismatch) \n output= %0x , expected=%0x \n",gpu_req.tmask,(int) strtol(value[3],NULL,16)) ;
					return 0;
				}
				if ( gpu_req.rd != (int) strtol(value[4],NULL,16) )
				{
					printf("FAILED (rd mismatch) \n output= %0x , expected=%0x \n",gpu_req.rd,(int) strtol(value[4],NULL,16)) ;
					return 0;
				}
			}

		}
    }
    fclose(fptr);
    fclose(fptr_out);
    printf("ALL TESTED CASES PASSED \n");

	return 0;
}

