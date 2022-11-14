#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue.h"



 dispatch  (VX_ibuffer_if ibuffer_if,bool* ready_ibuffer, VX_gpr_rsp_if gpr_rsp_if,
			VX_alu_req_if *alu_req_if, bool ready_gpu,
			VX_fpu_req_if *fpu_req_if, bool ready_fpu,
			VX_csr_req_if *csr_req_if, bool ready_csr,
			VX_lsu_req_if *lsu_req_if, bool ready_lsu,
			VX_gpu_req_if *gpu_req_if, bool ready_alu)

 {
	bool 		 alu_req_ready;
    bool 		 lsu_req_ready;
    bool 		 csr_req_ready;
    bool 		 fpu_req_ready;
    bool 		 gpu_req_ready;
	bool 		 alu_req_valid;
	bool 		 lsu_req_valid;
	bool 		 csr_req_valid;
	bool 		 fpu_req_valid;
	bool 		 gpu_req_valid;
	bool 		 alu_req_if_ready;
    bool 		 lsu_req_if_ready;
    bool 		 csr_req_if_ready;
    bool 		 fpu_req_if_ready;
    bool 		 gpu_req_if_ready;
	bool 		 alu_req_if_valid;
	bool 		 lsu_req_if_valid;
	bool 		 csr_req_if_valid;
	bool 		 fpu_req_if_valid;
	bool 		 gpu_req_if_valid;
	bool		 lsu_is_fence;
	bool		 lsu_is_prefetch;
	unsigned	 alu_op_type;
	unsigned	 lsu_op_type;
	unsigned	 csr_op_type;
	unsigned	 fpu_op_type;
	unsigned	 gpu_op_type;
	unsigned	 csr_addr;
	unsigned	 csr_imm;
	unsigned	 csr_rs1_data;
    bool   		 data_in_alu[DATAW_alu];
    bool   		 buffer_alu[DATAW_alu];
    bool   		 data_out_alu[DATAW_alu];
    bool   		 data_in_lsu[DATAW_lsu];
    bool   		 buffer_lsu[DATAW_lsu];
    bool   		 data_out_lsu[DATAW_lsu];
    bool   		 data_in_csr[DATAW_csr];
    bool   		 buffer_csr[DATAW_csr];
    bool   		 data_out_csr[DATAW_csr];
    bool   		 data_in_fpu[DATAW_fpu];
    bool   		 buffer_fpu[DATAW_fpu];
    bool   		 data_out_fpu[DATAW_fpu];
    bool   		 data_in_gpu[DATAW_gpu];
    bool   		 buffer_gpu[DATAW_gpu];
    bool   		 data_out_gpu[DATAW_gpu];
	char 		 tid;
	unsigned     next_PC;
	bool 		 push_alu;
	bool 		 pop_alu;
	bool 		 push_lsu;
	bool 		 pop_lsu;
	bool 		 push_csr;
	bool 		 pop_csr;
	bool 		 push_fpu;
	bool 		 pop_fpu;
	bool 		 push_gpu;
	bool 		 pop_gpu;


	//get thread ID
	for(int i = 0; i <NUM_THREADS; i++)
	{
		if(ibuffer_if.tmask [NUM_THREADS]==1)
		{
			 tid=i;
			break;
		}
	}

     // assign data_in_alu;
    alu_req_valid = ibuffer_if.valid && (ibuffer_if.ex_type == EX_ALU);
    alu_op_type = (ibuffer_if.op_type);

     for(int j = 0; j <UUID_BITS; j++) data_in_alu[j] = getBit (ibuffer_if.uuid,j) ;

     for(int j = UUID_BITS; j <UUID_BITS+NW_BITS; j++)
		 data_in_alu[j] = getBit (ibuffer_if.wid,j-UUID_BITS) ;

     for(int j = UUID_BITS+NW_BITS; j <UUID_BITS + NW_BITS + NUM_THREADS; j++)
		 data_in_alu[j] = getBit (ibuffer_if.tmask,j-(UUID_BITS+NW_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS; j <UUID_BITS + NW_BITS + NUM_THREADS + 32; j++)
        data_in_alu[j] = getBit (ibuffer_if.PC,j-(UUID_BITS + NW_BITS + NUM_THREADS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32; j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32; j++)
      data_in_alu[j] = getBit (ibuffer_if.next_PC,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS; j++)
      data_in_alu[j] = getBit (alu_op_type,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS; j++)
       data_in_alu[j] = getBit (ibuffer_if.op_mod,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32; j++)
      data_in_alu[j] = getBit (ibuffer_if.imm,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS)) ;

     for(int j =UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1; j++)
       data_in_alu[j] = getBit (ibuffer_if.use_PC,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1; j++)
      data_in_alu[j] = getBit (ibuffer_if.use_imm,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS; j++)
     data_in_alu[j] = getBit (ibuffer_if.rd,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1; j++)
       data_in_alu[j] = getBit (ibuffer_if.wb,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS; j++)
      data_in_alu[j] = getBit (tid,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS;
	 j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (1 * NUM_THREADS * 32);
	 j++)    data_in_alu[j] = getBit (gpr_rsp_if.rs1_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (1 * NUM_THREADS * 32);
	 j <DATAW_alu;
	 j++)    data_in_alu[j] = getBit (gpr_rsp_if.rs2_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (1 * NUM_THREADS * 32))) ;


     alu_req_if->uuid = getBits (data_out_alu , UUID_BITS-1,0) ;
     alu_req_if->wid   = getBits (data_out_alu , UUID_BITS+NW_BITS-1,UUID_BITS) ;
     //alu_req_if->tmask = getBits (data_out_alu , UUID_BITS + NW_BITS + NUM_THREADS-1,UUID_BITS+NW_BITS) ;
     alu_req_if->PC = getBits (data_out_alu ,UUID_BITS + NW_BITS + NUM_THREADS + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS) ;
     alu_req_if->next_PC = getBits (data_out_alu , UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32-1,
                                          UUID_BITS + NW_BITS + NUM_THREADS + 32) ;

     alu_req_if->op_type = getBits (data_out_alu , UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS-1,
                                    UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32) ;

     alu_req_if->op_mod = getBits (data_out_alu ,
                                   UUID_BITS+UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS) ;

     alu_req_if->imm = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS) ;

     alu_req_if->use_PC = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32) ;

     alu_req_if->use_imm = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1) ;


     alu_req_if->rd = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1) ;

     alu_req_if->wb = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS) ;

     alu_req_if->tid = getBits (data_out_alu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1) ;

     //alu_req_if->rs1_data = getBits (data_out_alu ,
       //                            UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (1 * NUM_THREADS * 32)-1,
         //                          UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS) ;

//	 alu_req_if->rs2_data = getBits (data_out_alu ,
  //                                 DATAW_alu-1,
    //                               UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_ALU_BITS + INST_MOD_BITS + 32 + 1 + 1 + NR_BITS + 1 + NT_BITS + (1 * NUM_THREADS * 32)) ;


     // assign data_in_lsu;
    lsu_req_valid = ibuffer_if.valid && (ibuffer_if.ex_type == EX_LSU);
    lsu_op_type = (ibuffer_if.op_type);
    lsu_is_fence = (ibuffer_if.op_mod);
    lsu_is_prefetch = (ibuffer_if.op_mod);
     for(int j = 0; j <UUID_BITS; j++) data_in_lsu[j] = getBit (ibuffer_if.uuid,j) ;
     for(int j = UUID_BITS; j <UUID_BITS+NW_BITS; j++)
		 data_in_lsu[j] = getBit (ibuffer_if.wid,j-UUID_BITS) ;

     for(int j = UUID_BITS+NW_BITS; j <UUID_BITS + NW_BITS + NUM_THREADS; j++)
		 data_in_lsu[j] = getBit (ibuffer_if.tmask,j-(UUID_BITS+NW_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS; j <UUID_BITS + NW_BITS + NUM_THREADS + 32; j++)
        data_in_lsu[j] = getBit (ibuffer_if.PC,j-(UUID_BITS + NW_BITS + NUM_THREADS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32; j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS; j++)
      data_in_lsu[j] = getBit (lsu_op_type,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1; j++)
      data_in_lsu[j] = getBit (lsu_is_fence,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32; j++)
       data_in_lsu[j] = getBit (ibuffer_if.imm,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS; j++)
      data_in_lsu[j] = getBit (ibuffer_if.rd,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32)) ;

     for(int j =UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1; j++)
       data_in_lsu[j] = getBit (ibuffer_if.wb,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (1 * NUM_THREADS * 32); j++)
      data_in_lsu[j] = getBit (gpr_rsp_if.rs1_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (1 * NUM_THREADS * 32);
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32); j++)
     data_in_lsu[j] = getBit (gpr_rsp_if.rs2_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (1 * NUM_THREADS * 32))) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32);
      j <DATAW_lsu; j++)
       data_in_lsu[j] = getBit (lsu_is_prefetch,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32))) ;


     lsu_req_if->uuid = getBits (data_out_lsu , UUID_BITS-1,0) ;
     lsu_req_if->wid   = getBits (data_out_lsu , UUID_BITS+NW_BITS-1,UUID_BITS) ;
    // lsu_req_if->tmask = getBits (data_out_lsu , UUID_BITS + NW_BITS + NUM_THREADS-1,UUID_BITS+NW_BITS) ;
     lsu_req_if->PC = getBits (data_out_lsu ,UUID_BITS + NW_BITS + NUM_THREADS + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS) ;

     lsu_req_if->op_type = getBits (data_out_lsu,UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS-1,
                                    UUID_BITS + NW_BITS + NUM_THREADS + 32) ;

     lsu_req_if->is_fence = getBits (data_out_lsu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS) ;

     lsu_req_if->offset = getBits (data_out_lsu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1) ;

     lsu_req_if->rd = getBits (data_out_lsu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32) ;

     lsu_req_if->wb = getBits (data_out_lsu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS) ;

     //lsu_req_if->base_addr = getBits (data_out_lsu ,
       //                            UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (1 * NUM_THREADS * 32)-1,
         //                          UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1) ;

    // lsu_req_if->store_data = getBits (data_out_lsu ,
      //                             UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32)-1,
        //                           UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (1 * NUM_THREADS * 32)) ;

	 lsu_req_if->is_prefetch = getBits (data_out_lsu ,
                                   DATAW_lsu-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_LSU_BITS + 1 + 32 + NR_BITS + 1 + (2 * NUM_THREADS * 32)) ;


	 // assign data_in_csr
    csr_req_valid = ibuffer_if.valid && (ibuffer_if.ex_type == EX_CSR);
    csr_op_type = (ibuffer_if.op_type);
    csr_addr = getBits(ibuffer_if.imm,CSR_ADDR_BITS-1,0);
	//imp
     csr_imm = getBits(ibuffer_if.imm,CSR_ADDR_BITS,NRI_BITS);
    csr_rs1_data = gpr_rsp_if.rs1_data[tid];

     for(int j = 0; j <UUID_BITS; j++) data_in_csr[j] = getBit (ibuffer_if.uuid,j) ;

     for(int j = UUID_BITS; j <UUID_BITS+NW_BITS; j++)
		 data_in_csr[j] = getBit (ibuffer_if.wid,j-UUID_BITS) ;

     for(int j = UUID_BITS+NW_BITS; j <UUID_BITS + NW_BITS + NUM_THREADS; j++)
		 data_in_csr[j] = getBit (ibuffer_if.tmask,j-(UUID_BITS+NW_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS; j <UUID_BITS + NW_BITS + NUM_THREADS + 32; j++)
        data_in_csr[j] = getBit (ibuffer_if.PC,j-(UUID_BITS + NW_BITS + NUM_THREADS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32; j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS; j++)
      data_in_csr[j] = getBit (csr_op_type,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS; j++)
      data_in_csr[j] = getBit (csr_addr,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS; j++)
       data_in_csr[j] = getBit (ibuffer_if.rd,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 ; j++)
      data_in_csr[j] = getBit (ibuffer_if.wb,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS)) ;

     for(int j =UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 ;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 ; j++)
       data_in_csr[j] = getBit (ibuffer_if.use_imm,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 )) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 ;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS; j++)
      data_in_csr[j] = getBit (csr_imm,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 )) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS;
     j <DATAW_csr; j++)
     data_in_csr[j] = getBit (csr_rs1_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS)) ;

     csr_req_if->uuid = getBits (data_out_csr , UUID_BITS-1,0) ;
     csr_req_if->wid   = getBits (data_out_csr , UUID_BITS+NW_BITS-1,UUID_BITS) ;
//     csr_req_if->tmask = getBits (data_out_csr , UUID_BITS + NW_BITS + NUM_THREADS-1,UUID_BITS+NW_BITS) ;
     csr_req_if->PC = getBits (data_out_csr ,UUID_BITS + NW_BITS + NUM_THREADS + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS) ;

     csr_req_if->op_type = getBits (data_out_csr,UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS-1,
                                    UUID_BITS + NW_BITS + NUM_THREADS + 32) ;

     csr_req_if->addr = getBits (data_out_csr ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS) ;

     csr_req_if->rd = getBits (data_out_csr ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS) ;

     csr_req_if->wb = getBits (data_out_csr ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS) ;

     csr_req_if->use_imm = getBits (data_out_csr ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1) ;

     csr_req_if->imm = getBits (data_out_csr ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1) ;

	// csr_req_if->rs1_data = getBits (data_out_csr ,
      //                             DATAW_csr-1,
        //                           UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_CSR_BITS + CSR_ADDR_BITS + NR_BITS + 1 + 1 + NRI_BITS) ;



	// assign data_in_fpu;
    fpu_req_valid = ibuffer_if.valid && (ibuffer_if.ex_type == EX_FPU);
    fpu_op_type = (ibuffer_if.op_type);
     for(int j = 0; j <UUID_BITS; j++) data_in_fpu[j] = getBit (ibuffer_if.uuid,j) ;

     for(int j = UUID_BITS; j <UUID_BITS+NW_BITS; j++)
		 data_in_fpu[j] = getBit (ibuffer_if.wid,j-UUID_BITS) ;

     for(int j = UUID_BITS+NW_BITS; j <UUID_BITS + NW_BITS + NUM_THREADS; j++)
		 data_in_fpu[j] = getBit (ibuffer_if.tmask,j-(UUID_BITS+NW_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS; j <UUID_BITS + NW_BITS + NUM_THREADS + 32; j++)
        data_in_fpu[j] = getBit (ibuffer_if.PC,j-(UUID_BITS + NW_BITS + NUM_THREADS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32; j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS; j++)
      data_in_fpu[j] = getBit (fpu_op_type,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS; j++)
      data_in_fpu[j] = getBit (ibuffer_if.op_mod,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS; j++)
       data_in_fpu[j] = getBit (ibuffer_if.rd,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1; j++)
      data_in_fpu[j] = getBit (ibuffer_if.wb,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS)) ;

     for(int j =UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (1 * NUM_THREADS * 32); j++)
       data_in_fpu[j] = getBit (gpr_rsp_if.rs1_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (1 * NUM_THREADS * 32);
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (2 * NUM_THREADS * 32); j++)
      data_in_fpu[j] = getBit (gpr_rsp_if.rs2_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (1 * NUM_THREADS * 32))) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (2 * NUM_THREADS * 32);
     j <DATAW_fpu; j++)
     data_in_fpu[j] = getBit (gpr_rsp_if.rs3_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (2 * NUM_THREADS * 32))) ;


     fpu_req_if->uuid = getBits (data_out_fpu , UUID_BITS-1,0) ;
     fpu_req_if->wid   = getBits (data_out_fpu , UUID_BITS+NW_BITS-1,UUID_BITS) ;
  //   fpu_req_if->tmask = getBits (data_out_fpu , UUID_BITS + NW_BITS + NUM_THREADS-1,UUID_BITS+NW_BITS) ;
     fpu_req_if->PC = getBits (data_out_fpu ,UUID_BITS + NW_BITS + NUM_THREADS + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS) ;

     fpu_req_if->op_type = getBits (data_out_fpu,UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS-1,
                                    UUID_BITS + NW_BITS + NUM_THREADS + 32) ;

     fpu_req_if->op_mod = getBits (data_out_fpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS) ;

     fpu_req_if->rd = getBits (data_out_fpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS) ;

     fpu_req_if->wb = getBits (data_out_fpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS) ;

   //  fpu_req_if->rs1_data = getBits (data_out_fpu ,
     //                              UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (1 * NUM_THREADS * 32)-1,
       //                            UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1) ;

    // fpu_req_if->rs2_data = getBits (data_out_fpu ,
      //                             UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (2 * NUM_THREADS * 32)-1,
        //                           UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (1 * NUM_THREADS * 32)) ;

//	 fpu_req_if->rs3_data = getBits (data_out_fpu ,
  //                                 DATAW_fpu-1,
    //                               UUID_BITS + NW_BITS + NUM_THREADS + 32 + INST_FPU_BITS + INST_MOD_BITS + NR_BITS + 1 + (2 * NUM_THREADS * 32)) ;


	// assign data_in_gpu;
    gpu_req_valid = ibuffer_if.valid && (ibuffer_if.ex_type == EX_GPU);
    gpu_op_type = (ibuffer_if.op_type);
     for(int j = 0; j <UUID_BITS; j++) data_in_gpu[j] = getBit (ibuffer_if.uuid,j) ;

     for(int j = UUID_BITS; j <UUID_BITS+NW_BITS; j++)
		 data_in_gpu[j] = getBit (ibuffer_if.wid,j-UUID_BITS) ;

     for(int j = UUID_BITS+NW_BITS; j <UUID_BITS + NW_BITS + NUM_THREADS; j++)
		 data_in_gpu[j] = getBit (ibuffer_if.tmask,j-(UUID_BITS+NW_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS; j <UUID_BITS + NW_BITS + NUM_THREADS + 32; j++)
        data_in_gpu[j] = getBit (ibuffer_if.PC,j-(UUID_BITS + NW_BITS + NUM_THREADS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32; j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32; j++)
      data_in_gpu[j] = getBit (ibuffer_if.next_PC,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS; j++)
      data_in_gpu[j] = getBit (gpu_op_type,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS; j++)
       data_in_gpu[j] = getBit (ibuffer_if.op_mod,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS; j++)
      data_in_gpu[j] = getBit (ibuffer_if.rd,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS)) ;

     for(int j =UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1; j++)
       data_in_gpu[j] = getBit (ibuffer_if.wb,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1;
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1 + NT_BITS; j++)
      data_in_gpu[j] = getBit (tid,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1 + NT_BITS;
     j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32); j++)
     data_in_gpu[j] = getBit (gpr_rsp_if.rs1_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1 + NT_BITS)) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32);
      j <UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (2 * NUM_THREADS * 32); j++)
       data_in_gpu[j] = getBit (gpr_rsp_if.rs2_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32))) ;

     for(int j = UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (2 * NUM_THREADS * 32);
      j <DATAW_gpu; j++)
      data_in_gpu[j] = getBit (gpr_rsp_if.rs3_data,j-(UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (2 * NUM_THREADS * 32))) ;


     gpu_req_if->uuid = getBits (data_out_gpu , UUID_BITS-1,0) ;
     gpu_req_if->wid   = getBits (data_out_gpu , UUID_BITS+NW_BITS-1,UUID_BITS) ;
    // gpu_req_if->tmask = getBits (data_out_gpu , UUID_BITS + NW_BITS + NUM_THREADS-1,UUID_BITS+NW_BITS) ;
     gpu_req_if->PC = getBits (data_out_gpu ,UUID_BITS + NW_BITS + NUM_THREADS + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS) ;
     gpu_req_if->next_PC = getBits (data_out_gpu ,UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32-1,
                                           UUID_BITS + NW_BITS + NUM_THREADS + 32 ) ;

     gpu_req_if->op_type = getBits (data_out_gpu,UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS-1,
                                    UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32) ;

     gpu_req_if->op_mod = getBits (data_out_gpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS) ;

     gpu_req_if->rd = getBits (data_out_gpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS) ;

     gpu_req_if->wb = getBits (data_out_gpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS) ;

     gpu_req_if->tid = getBits (data_out_gpu ,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1 + NT_BITS-1,
                                   UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1) ;

    // gpu_req_if->rs1_data = getBits (data_out_gpu ,
      //                             UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32)-1,
        //                           UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + 32 + 1 + NT_BITS) ;

     //gpu_req_if->rs2_data = getBits (data_out_gpu ,
       //                            UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (2 * NUM_THREADS * 32)-1,
         //                          UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32)) ;

	// gpu_req_if->rs3_data = getBits (data_out_gpu ,
      //                             DATAW_gpu-1,
        //                           UUID_BITS + NW_BITS + NUM_THREADS + 32 + 32 + INST_GPU_BITS + INST_MOD_BITS + NR_BITS + 1 + NT_BITS  + (1 * NUM_THREADS * 32)) ;





        //skid_buffer alu
		push_alu = alu_req_valid && alu_req_ready;
        pop_alu = !alu_req_if_valid || alu_req_if_ready;
		if (alu_req_if_ready) 						 alu_req_ready = 1;
		else if (alu_req_valid  && alu_req_if_valid) alu_req_ready = 0;
	    if (pop_alu)  								 alu_req_if_valid = alu_req_valid || !alu_req_ready;
        if (push_alu) 								 for(int j = 0; j <DATAW_alu; j++) buffer_alu[j] = data_in_alu[j];
		if (pop_alu && alu_req_ready)    			 for(int j = 0; j <DATAW_alu; j++) data_out_alu[j] = data_in_alu[j];
		else if	 (alu_req_if_ready)		 			 for(int j = 0; j <DATAW_alu; j++)  data_out_alu[j] = buffer_alu[j];


        //skid_buffer gpu
		push_gpu = gpu_req_valid && gpu_req_ready;
        pop_gpu = !gpu_req_if_valid || gpu_req_if_ready;
		if (gpu_req_if_ready) 						 gpu_req_ready = 1;
		else if (gpu_req_valid  && gpu_req_if_valid) gpu_req_ready = 0;
	    if (pop_gpu)  								 gpu_req_if_valid = gpu_req_valid || !gpu_req_ready;
        if (push_gpu) 								 for(int j = 0; j <DATAW_gpu; j++) buffer_gpu[j] = data_in_gpu[j];
		if (pop_gpu && gpu_req_ready)   			 for(int j = 0; j <DATAW_gpu; j++) data_out_gpu[j] = data_in_gpu[j];
		else if	 (gpu_req_if_ready)					 for(int j = 0; j <DATAW_gpu; j++)  data_out_gpu[j] = buffer_gpu[j];

        //skid_buffer fpu
		push_fpu = fpu_req_valid && fpu_req_ready;
        pop_fpu = !fpu_req_if_valid || fpu_req_if_ready;
		if (fpu_req_if_ready) 						 fpu_req_ready = 1;
		else if (fpu_req_valid  && fpu_req_if_valid) fpu_req_ready = 0;
	    if (pop_fpu)  								 fpu_req_if_valid = fpu_req_valid || !fpu_req_ready;
        if (push_fpu) 								 for(int j = 0; j <DATAW_fpu; j++) buffer_fpu[j] = data_in_fpu[j];
		if (pop_fpu && fpu_req_ready)   			 for(int j = 0; j <DATAW_fpu; j++) data_out_fpu[j] = data_in_fpu[j];
		else if	 (fpu_req_if_ready)					 for(int j = 0; j <DATAW_fpu; j++)  data_out_fpu[j] = buffer_fpu[j];


        //skid_buffer csr
		push_csr = csr_req_valid && csr_req_ready;
        pop_csr = !csr_req_if_valid || csr_req_if_ready;
		if (csr_req_if_ready) 						 csr_req_ready = 1;
		else if (csr_req_valid  && csr_req_if_valid) csr_req_ready = 0;
	    if (pop_csr)  								 csr_req_if_valid = csr_req_valid || !csr_req_ready;
        if (push_csr) 								 for(int j = 0; j <DATAW_csr; j++) buffer_csr[j] = data_in_csr[j];
		if (pop_csr && csr_req_ready)    			 for(int j = 0; j <DATAW_csr; j++) data_out_csr[j] = data_in_csr[j];
		else if	 (csr_req_if_ready)					 for(int j = 0; j <DATAW_csr; j++)  data_out_csr[j] = buffer_csr[j];

        //skid_buffer lsu
		push_lsu = lsu_req_valid && lsu_req_ready;
        pop_lsu = !lsu_req_if_valid || lsu_req_if_ready;
		if (lsu_req_if_ready) 						 lsu_req_ready = 1;
		else if (lsu_req_valid  && lsu_req_if_valid) lsu_req_ready = 0;
	    if (pop_lsu)  								 lsu_req_if_valid = lsu_req_valid || !lsu_req_ready;
        if (push_lsu) 							     for(int j = 0; j <DATAW_lsu; j++) buffer_lsu[j] = data_in_lsu[j];
		if (pop_lsu && lsu_req_ready)    			 for(int j = 0; j <DATAW_lsu; j++) data_out_lsu[j] = data_in_lsu[j];
		else if	 (lsu_req_if_ready)					 for(int j = 0; j <DATAW_lsu; j++)  data_out_lsu[j] = buffer_lsu[j];


 }

//====================================================================================================================
