//=====================================================================
//
// decoder.c                                              Date: 2022/10/29
//
// Author : Mustafa abdelhamid
//=====================================================================


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "decoder.h"
#include "common.h"

// registers naming conventioal ( Application binary interface)
char registers_abi [32] [5] = {
	"zero", 	//x0
	"ra",		//x1
	"sp",		//x2
	"gp",		//x3
	"tp",		//x4
	"t0", 		//x5
	"t1",		//x6
	"t2",		//x7
	"s0",		//x8
	"s1",		//x9
	"a0",	 	//x10
	"a1",		//x11
	"a2",		//x12
	"a3",		//x13
	"a4",		//x14
	"a5", 		//x15
	"a6",		//x16
	"a7",		//x17
	"s2",		//x18
	"s3",		//x19
	"s4",		//x20
	"s5",		//x21
	"s6",		//x22
	"s7",		//x23
	"s8",	 	//x24
	"s9",		//x25
	"s10",		//x26
	"s11",		//x27
	"t3",		//x28
	"t4", 		//x29
	"t5",		//x30
	"t6"		//x31

} ;

char *get_regName(uint16_t regNum) {
	return registers_abi[regNum] ;
}

uint32_t extendSign ( uint32_t signal , uint8_t signalWidth)  {

	uint32_t extendMask= 0xFFFFFFFF;
	extendMask = extendMask << signalWidth ;
	bool checkPolarity = ( signal >> (signalWidth-1) ) ;
	if (!checkPolarity)
		return (signal) ;
	else
		return (extendMask | signal) ;
	/*
	#ifdef DEBUG
	printf("signal: 0x%04x\n", signal);
	printf("extendMask: 0x%04x\n", extendMask);
	printf("extended signal: 0x%08x\n", extendedSignal);
	#endif
	*/
}

//====================================================================================================================

void decode (instr_fetch_rsp fetched_instruction,
 perf_decode_if *decoder_performance, VX_decode *decoded_signals , wstall_if *stalled_warps , join_if *join_inst )
{
	//************************** pass undecoded signals ***********************************//
	decoded_signals -> uuid = fetched_instruction.uuid;
	decoded_signals -> wid = fetched_instruction.wid ;
        decoded_signals -> tmask =fetched_instruction.tmask;
        decoded_signals -> PC= fetched_instruction.pc;

	stalled_warps -> wid = fetched_instruction.wid ;

	join_inst -> wid = fetched_instruction.wid ;

//************************** initialize decoded signals  ***********************************//
// NOTE : performance counter ISN'T initialized ... need to be initialized in main
	decoded_signals -> ex_type=0;
        decoded_signals -> op_type=0;
        decoded_signals -> op_mod=0;
        decoded_signals -> wb=0;
        decoded_signals -> use_PC=0;
        decoded_signals -> use_imm=0;
        decoded_signals -> imm=0;
        decoded_signals -> rd=0;
        decoded_signals -> rs1=0;
        decoded_signals -> rs2=0;
        decoded_signals -> rs3=0;

	stalled_warps -> valid = 0 ;
	stalled_warps -> stalled = 0 ;

	join_inst -> valid = 0 ;
//**************************************************************************************************//
//***************************** EXTRACT Informations from instruction ***********************************//
	// for  signal [x:y] ... signal>>(y) & mask of ones equal Width

	uint8_t opcode 	= 	getBits(fetched_instruction.instr,6,0);	//instr [6:0]
	uint8_t func2  	= 	getBits(fetched_instruction.instr,26,25);	//instr[26:25];
	uint8_t func3  	= 	getBits(fetched_instruction.instr,14,12);	//instr[14:12];
	uint8_t func7  	= 	getBits(fetched_instruction.instr,31,25);	//instr[31:25];
	uint16_t u_12  	= 	getBits(fetched_instruction.instr,31,20);	//instr[31:20];
	uint8_t rd  		= 	getBits(fetched_instruction.instr,11,7);	//instr[11:7];
	uint8_t rs1  		= 	getBits(fetched_instruction.instr,19,15);	//instr[19:15];
	uint8_t rs2  		= 	getBits(fetched_instruction.instr,24,20);	//instr[24:20];
	uint8_t rs3  		= 	getBits(fetched_instruction.instr,31,27);	//instr[31:27];

	// immediate values
	// 20 bit U tybe immediate
	uint32_t upper_imm  	= 	getBits(fetched_instruction.instr,31,12);	//instr[31:12];

	/* ALU immediate in case of shift operation
	(we need only 5 bits: max shift is 32   so riscv make use of the rest of imm bits)
	------------------------------------------------------------------------------------------------------
	instr	||	func3			|| imm			||	description 			||
	------	||---------------------------	||------------------	||-----------------------------------	||
	slli	||  	0x1			|| imm [11:5] = 0x00 	||	rd = rs1 << imm[4:0](rs2)	||
	------	||---------------------------	||--------------------	||-----------------------------------	||
	slri	||  	0x5			|| imm [11:5] = 0x00 	||	rd = rs1 >> imm[0:4]		||
	------	||---------------------------	||--------------------	||-----------------------------------	||
	srla	||	0x5			|| imm [11:5] = 0x20	||	rd = rs1 >> imm[0:4]		||
	--------------------------------------------------------------------------------------------------	||
	*/
	uint16_t alu_imm ;
    if (func3==5 || func3==1 ) {
		alu_imm= u_12 & 0x01F;
	}
	else {
		alu_imm=u_12;
	}

	// s type immediate {func7, rd};
	uint16_t s_imm = func7 ;
	s_imm = (s_imm <<5) | rd;
    //********************
	//b_imm   = {instr[31], instr[7], instr[30:25], instr[11:8], 1'b0};
	uint16_t b_imm = concatenate( getBit(fetched_instruction.instr,31) ,1, getBit(fetched_instruction.instr,7),1)  ;
	b_imm = concatenate (b_imm,2,getBits(fetched_instruction.instr,30,25),6);
	b_imm = concatenate (b_imm,8,getBits(fetched_instruction.instr,11,8),4);
	b_imm = concatenate (b_imm,12,0x0,1);

	// jal_imm   = {instr[31], instr[19:12], instr[20], instr[30:21], 1'b0};
	uint32_t jal_imm = concatenate (getBit(fetched_instruction.instr,31),1,getBits(fetched_instruction.instr,19,12),8) ;
	jal_imm=concatenate(jal_imm,9,getBit(fetched_instruction.instr,20),1);
	jal_imm=concatenate(jal_imm,10,getBits(fetched_instruction.instr,30,21),10);
	jal_imm=concatenate(jal_imm,20,0x0,1);
	/*
	#ifdef DEBUG
	printf("************************* \nInstruction :0x%08x\n", fetched_instruction.instr);
	printf("Opcode: 0x%02x\n", opcode);
	printf("func2 :0x%01x\n", func2);
	printf("func3: 0x%01x\n", func3);
	printf("func7 :0x%02x\n", func7);
	printf("rd :0x%02x\n", rd);
	printf("rs1: 0x%02x\n", rs1);
	printf("rs2: 0x%02x\n", rs2);
	printf("rs3: 0x%02x\n", rs3);

	printf("u_12: 0x%03x\n", u_12);
	printf("b_imm: 0x%08x\n", b_imm);
	printf("jal_imm: 0x%08x\n", jal_imm);
	#endif
	*/

	char *rd_name= get_regName(rd);
	char *rs1_name= get_regName(rs1);
	char *rs2_name= get_regName(rs2);
	char *rs3_name= get_regName(rs3);

	char *f_rd_name= get_regName(rd+32); // floating point register {1,rd}
	char *f_rs1_name= get_regName(rs1+32);
	char *f_rs2_name= get_regName(rs2+32);
	char *f_rs3_name= get_regName(rs3+32);

//*********************************** DECODER *****************************************//
//=======================================================================================
	switch(opcode) {

//****************************	 I type 	*****************************//
    case INST_I  :

	decoded_signals -> ex_type=EX_ALU;
        decoded_signals -> use_imm=1;
        decoded_signals -> imm=extendSign(alu_imm,12);
        decoded_signals -> rd= rd;
        decoded_signals -> rs1=rs1;

        //write back value
	if (rd !=0)
		decoded_signals -> wb=1;
	else
		decoded_signals -> wb=0;


	// optype and deassemble
	switch (func3) {
		case 0x0:
			// intented for deassemble output
			printf("addi %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_ADD;
			break;
		case 0x1:
			printf("slli %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_SLL;
			break;
		case 0x2:
			printf("slti %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_SLT;
			break;
		case 0x3:
			printf("sltiu %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_SLTU;
			break;
		case 0x4:

			printf("xori %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_XOR;
			break;
		case 0x5:

			printf("srai %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_SRA;
			break;
		case 0x6:

			printf("ori %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_OR;
			break;
		case 0x7:

			printf("andi %s , %s , %d \n",rd_name,rs1_name,decoded_signals -> imm);
			decoded_signals -> op_type= INST_ALU_AND;
			break;
	}

	break;
//****************************	 R type 	*****************************//
	case INST_R  :

	decoded_signals -> ex_type=EX_ALU;

        decoded_signals -> rd= rd;
        decoded_signals -> rs1=rs1;
        decoded_signals -> rs2=rs2;

        //write back value
	if (rd !=0)
		decoded_signals -> wb=1;
	else
		decoded_signals -> wb=0;



	// optype and deassemble
	// Multiply extension
	if (func7==0x01) {
		decoded_signals -> op_mod = 2 ;
		switch(func3) {
			case 0x0:
				printf("mul %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_MUL;
				break;
			case 0x1:
				printf("mulh %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_MULH;
				break;
			case 0x2:
				printf("mulhsu %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_MULHSU;
				break;
			case 0x3:
				printf("mulhu %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_MULHU;
				break;
			case 0x4:
				printf("div %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_DIV;
				break;
			case 0x5:
				printf("divu %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_DIVU;
				break;
			case 0x6:
				printf("rem %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_REM;
				break;
			case 0x7:
				printf("remu %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type = INST_MUL_REMU;
				break;

			}
		} else {// ALU operation
			switch (func3) {
		case 0x0:
			// intented for deassemble output
			if (func7==0) {
				printf("add %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type= INST_ALU_ADD;
				}
			else {
				printf("sub %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type= INST_ALU_ADD;
			}
			break;
		case 0x1:
			printf("sll %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_SLL;
			break;
		case 0x2:
			printf("slt %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_SLT;
			break;
		case 0x3:
			printf("sltu %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_SLTU;
			break;
		case 0x4:

			printf("xor %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_XOR;
			break;
		case 0x5:
			if (func7==0) {
				printf("srl %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type= INST_ALU_SRL;
			} else {
				printf("sra %s , %s , %s \n",rd_name,rs1_name,rs2_name);
				decoded_signals -> op_type= INST_ALU_SRA;
			}
			break;
		case 0x6:

			printf("or %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_OR;
			break;
		case 0x7:

			printf("and %s , %s , %s \n",rd_name,rs1_name,rs2_name);
			decoded_signals -> op_type= INST_ALU_AND;
			break;
	}
		}


	break;
//****************************	 U type 	*****************************//
// lui
   case INST_LUI:
   	decoded_signals -> ex_type = EX_ALU;
        decoded_signals ->op_type = INST_ALU_LUI;
        decoded_signals ->op_mod  = 1;
        decoded_signals ->wb  = (rd==0) ? 0 : 1;
        decoded_signals ->use_imm = 1;


        decoded_signals ->imm = upper_imm << 12;
        decoded_signals -> rd=rd;

        stalled_warps -> stalled = 1 ;
	//x[rd] <--- upper_imm << 12
	printf("lui %s , %d \n",rd_name,upper_imm);
   	break ;

// auipc
   case INST_AUIPC:
   	decoded_signals -> ex_type = EX_ALU;
        decoded_signals ->op_type = INST_ALU_AUIPC;
        decoded_signals ->op_mod  = 1;
        decoded_signals ->wb  = (rd==0) ? 0 : 1;
        decoded_signals ->use_imm = 1;
	decoded_signals ->use_PC= 1;

        decoded_signals ->imm = upper_imm << 12;
        decoded_signals -> rd=rd;

        stalled_warps -> stalled = 1 ;
   	// x[rd] <--- pc +  upper_imm << 12
   	printf("auipc %s , %d \n",rd_name,upper_imm );
   	break ;
//****************************	 JAL instr 	*****************************//
   case INST_JAL:
   	decoded_signals -> ex_type = EX_ALU;
        decoded_signals ->op_type = INST_BR_JAL;
        decoded_signals ->op_mod  = 1;
        decoded_signals ->wb  = (rd==0) ? 0 : 1;
        decoded_signals ->use_imm = 1;
	decoded_signals ->use_PC= 1;

        decoded_signals ->imm = extendSign(jal_imm,21);
        decoded_signals -> rd=rd;

        stalled_warps -> stalled = 1 ;
   	// x[rd] <--- pc+4
   	// pc <--- pc+ signExtend(offset)
   	printf("jal %s , %d \n",rd_name,jal_imm);

   	break ;
//****************************	 JALR instr 	*****************************//
   case INST_JALR:
   	decoded_signals -> ex_type = EX_ALU;
        decoded_signals ->op_type = INST_BR_JALR;
        decoded_signals ->op_mod  = 1;
        decoded_signals ->wb  = (rd==0) ? 0 : 1;
        decoded_signals ->use_imm = 1;
        decoded_signals ->imm = extendSign(u_12,12);
        decoded_signals -> rd=rd;
        decoded_signals -> rs1=rs1;
        stalled_warps -> stalled = 1 ;
   	// x[rd] <--- pc+4
   	// pc <--- x[rs1] + signExtend(offset)
   	printf("jalr %s , %s , %d \n",rd_name,rs1_name,decoded_signals ->imm );
   	break ;
//****************************	 B instr 	*****************************//
   case INST_B:
   	decoded_signals -> ex_type = EX_ALU;
        decoded_signals ->op_mod  = 1;
        decoded_signals ->use_imm = 1;
        decoded_signals ->use_PC= 1;
        decoded_signals ->imm = extendSign(b_imm,13);

  	decoded_signals -> rs1=rs1;
  	decoded_signals -> rs2=rs2;

        stalled_warps -> stalled = 1 ;
        switch(func3) {
        	case 0x0:
   			decoded_signals ->op_type = INST_BR_EQ;
   			printf("beq  %s , %s , %d \n",rs1_name,rs2_name,b_imm);
   			break ;
   		case 0x1:
   			decoded_signals ->op_type = INST_BR_NE;
   			printf("bne  %s , %s , %d \n",rs1_name,rs2_name,decoded_signals ->imm );
   			break ;
   		case 0x4:
   			decoded_signals ->op_type = INST_BR_LT;
   			printf("blt  %s , %s , %d \n",rs1_name,rs2_name,decoded_signals ->imm );
   			break ;
   		case 0x5:
   			decoded_signals ->op_type = INST_BR_GE;
   			printf("bge %s , %s , %d \n",rs1_name,rs2_name,decoded_signals ->imm );
   			break ;
   		case 0x6:
   			decoded_signals ->op_type = INST_BR_LTU;
   			printf("bltu %s , %s , %d \n",rs1_name,rs2_name,decoded_signals ->imm );
   			break ;
   		case 0x7:
   			decoded_signals ->op_type = INST_BR_GEU;
   			printf("bgeu %s , %s , %d \n",rs1_name,rs2_name,decoded_signals ->imm );
   			break ;
   		 }
   	break ;
   //****************************	 FENCE instr 	*****************************//
   case INST_FENCE:
   	decoded_signals -> ex_type = EX_LSU;
   	decoded_signals ->op_mod  = 1;
   	break ;
   //****************************	 SYS instr 	*****************************//
   case INST_SYS: // CSR and SYS CALLS
   	switch (func3) {
   		case 0x0: // ECALL
   			decoded_signals -> ex_type = EX_ALU;
 			decoded_signals ->op_mod  = 1;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 1;
			decoded_signals ->use_PC = 1;
			decoded_signals ->imm = 4;
			decoded_signals -> rd=rd;

		  	stalled_warps -> stalled = 1 ;
		  	switch(u_12){
		  		case 0x000: //ecall
		  			decoded_signals ->op_type  = INST_BR_ECALL;
		  			printf("ecall");
		  			break ;
		  		case 0x001: //ebreak
		  			decoded_signals ->op_type  = INST_BR_EBREAK;
		  			printf("ebreak");
		  			break ;
		  		case 0x102:
		  			decoded_signals ->op_type  = INST_BR_SRET;
		  			printf("sret");
		  			break ;
		  		case 0x302:
		  			decoded_signals ->op_type  = INST_BR_MRET;
		  			printf("mret");
		  			break ;
		  		case 0x105: //wait interrupt  < not implemented >
		  			break ;
		  	}

		  	break ;
		  case 0x1: // csrrw read/write
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 1;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals -> rd=rd;
		  	decoded_signals -> rs1=rs1;
		  	printf("csrrw  %s , %d , %s \n",rd_name,decoded_signals ->imm,rs1_name );

		  	break ;
		  case 0x2: // csrrs read/set
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 2;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals -> rd=rd;
		  	decoded_signals -> rs1=rs1;
		  	printf("csrrs  %s , %d , %s \n",rd_name,decoded_signals ->imm,rs1_name );

		  	break ;
		  case 0x3: // csrrc read/clear
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 3;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals -> rd=rd;
		  	decoded_signals -> rs1=rs1;
		  	printf("csrrc  %s , %d , %s \n",rd_name,decoded_signals ->imm,rs1_name );

		  	break ;
		  case 0x4:

		  case 0x5: // csrrwi
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 1;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals ->imm = ((decoded_signals ->imm) <<5) | rs1 ;
			decoded_signals -> rd=rd;
		  	// csrrwi rd,offset,uimm
		  	printf("csrrwi  %s , %d , %d \n",rd_name,u_12, rs1);

		  	break ;
		  case 0x6: // csrrsi
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 2;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals ->imm = ((decoded_signals ->imm) <<5) | rs1 ;
			decoded_signals -> rd=rd;
		  	// csrrsi rd,offset,uimm
		  	printf("csrrsi  %s , %d , %d \n",rd_name,u_12, rs1);
		  	break ;

		  case 0x7: // csrrci
   			decoded_signals -> ex_type = EX_CSR;
			decoded_signals ->op_type  = 3;
			decoded_signals ->wb  = (rd==0) ? 0 : 1;
			decoded_signals ->use_imm = 0;
			decoded_signals ->imm = u_12; //csr address
			decoded_signals ->imm = ((decoded_signals ->imm) <<5) | rs1 ;
			decoded_signals -> rd=rd;
		  	// csrrsi rd,offset,uimm
		  	printf("csrrci  %s , %d , %d \n",rd_name,u_12, rs1);
		  	break ;
   	}

   	break ;
   //****************************	 Load instr instr 	*****************************//
   case INST_FL :
   case INST_L:
   	decoded_signals -> ex_type = EX_LSU;
   	decoded_signals ->op_type = concatenate(0x0,1,func3,3) ;
   	decoded_signals -> wb = (rd==0) ? 0 : 1;
   	decoded_signals -> imm = extendSign(u_12,12) ;
   	decoded_signals -> rs1 = rs1 ;
   	if (getBit(opcode,2)) {// floating point load
   		decoded_signals -> rd = rd+32 ; // {1'b1,rd}
   		//decoded_signals -> rs1 = rs1+32 ; // {1'b1,rs1} (xx rs1 is an integer register)

   		// f[rd] = M [ x[rs1] + extendSign (u_12)]
   		printf("flw  %s , %d ( %s ) \n",f_rd_name,u_12, rs1_name);
   	} else { // integer load
   		decoded_signals -> rd = rd ;
   		switch(func3) {
   			case 0x0: // load 8 bit value and sign extend to XLEN
   				printf("lb %s , %d ( %s ) \n",rd_name,u_12,rs1_name);
   				break;
			case 0x1: // load 16 bit value and sign extend to XLEN
   				printf("lh %s , %d ( %s ) \n",rd_name,u_12,rs1_name);
   				break;
			case 0x2: // load 32 bit value and sign extend to XLEN
   				printf("lw %s , %d ( %s ) \n",rd_name,u_12,rs1_name);
   				break;
			case 0x4: // load 8 bit value and  extend to XLEN
   				printf("lbu %s , %d ( %s ) \n",rd_name,u_12,rs1_name);
   				break;
			case 0x5:// load 16 bit value and  extend to XLEN
   				printf("lhu %s , %d ( %s ) \n",rd_name,u_12,rs1_name);
   				break;

   		}
   	}


   	break ;
   //****************************	 Load instr instr 	*****************************//
   case INST_FS :
   case INST_S:
   	decoded_signals -> ex_type = EX_LSU;
   	decoded_signals ->op_type = concatenate(0x1,1,func3,3) ;

   	decoded_signals -> imm = extendSign(s_imm,12) ;
   	decoded_signals -> rs1 = rs1 ;
   	if (getBit(opcode,2)) {// floating point store
   		decoded_signals -> rd = rd+32 ; // {1'b1,rd}

   		// M [ f[rs1] + extendSign (s_imm)] = x[rs1][7:0]

   		printf("flw  %s , %d ( %s ) \n",f_rd_name,u_12, rs1_name);
   	} else { // integer store
   		decoded_signals -> rd = rd ;
   		switch(func3) {
   			case 0x0: // store 8 bit value
   				// M [ x[rs1] + extendSign [s_imm] ] = 8' x[rs2]
   				printf("sb %s , %d ( %s ) \n",rs2_name,s_imm,rs1_name);
   				break;
			case 0x1: // store 16 bit value
   				// M [ x[rs1] + extendSign [s_imm] ] = 16' x[rs2]
   				printf("sh %s , %d ( %s ) \n",rs2_name,s_imm,rs1_name);
			case 0x2: // store word
   				// M [ x[rs1] + extendSign [s_imm] ] =  x[rs2]
   				printf("sw %s , %d ( %s ) \n",rs2_name,s_imm,rs1_name);
				break ;

   		}
   	}


   	break ;
   //****************************	 Floating instr 	*****************************//
   case INST_FMADD:
   case INST_FMSUB:
   case INST_FNMSUB:
   case INST_FNMADD:
   	decoded_signals -> ex_type = EX_FPU;
   	decoded_signals ->op_type = getBits(opcode,3,0) ;
   	decoded_signals -> op_mod = func3 ;
	decoded_signals ->wb  = (rd==0) ? 0 : 1;
   	// floating point registers
   	decoded_signals -> rd = rd+32 ;
   	decoded_signals -> rs1 = rs1+32 ;
   	decoded_signals -> rs2 = rs2+32 ;
   	decoded_signals -> rs3 = rs3+32 ;

   	switch(getBits(opcode,3,2)) {
   			case 0x0: // FMADD
   				//
   				printf("fmadd.s %s , %s ,%s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name,f_rs3_name);
   				break;
			case 0x1: // FMSUB

   				printf("fmsub.s %s , %s ,%s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name,f_rs3_name);
   				break;
			case 0x2: // FNMSUB
   				// M [ x[rs1] + extendSign [s_imm] ] =  x[rs2]
   				printf("fnmsub.s %s , %s ,%s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name,f_rs3_name);
				break ;
			case 0x3: // FNMADD

   				printf("fnmadd.s %s , %s ,%s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name,f_rs3_name);
				break ;

   		}
   	break ;
   //****************************	 FCI instr 	*****************************//
   case INST_FCI: // rest of floating point instr
   	decoded_signals -> ex_type = EX_FPU;
   	decoded_signals -> op_mod = func3 ;
	decoded_signals ->wb  = (rd==0) ? 0 : 1;

	switch(func7) {
   			case 0x00: // fadd
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = 0x0 ;
   				printf("fadd.s %s , %s ,%s  \n",f_rd_name,f_rs1_name,f_rs2_name);
   				break;
			case 0x04: // fsub
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = 0x4 ;
   				printf("fsub.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   				break;
			case 0x08: // fmul
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = 0x8 ;
   				printf("fmul.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   				break;
			case 0x0C: // fdiv
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = 0xC ;
   				printf("fdiv.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   				break;
			//********************************************//
   			case 0x2C: // fsqrt
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;


   				decoded_signals ->op_type = INST_FPU_SQRT ;

   				printf("fsqrt.s %s , %s \n",f_rd_name,f_rs1_name);
   				break;
			case 0x10: // SGN instructions
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = INST_FPU_MISC ;
   				switch (func3) {
   					case 0x0://fsgnj .. f[rd] = f[rs1][30]  but with rs2 sign
   						decoded_signals ->op_mod = 0x0 ;
   						printf("fsgnj.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   						break;
   					case 0x1://fsgnjn f[rd] = f[rs1][30]  but with negative rs2 sign
   						decoded_signals ->op_mod = 0x1 ;
   						printf("fsgnjn.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   						break;
   					case 0x2://fsgnjx f[rd] = f[rs1][30]  but with  rs2 sign * rs1 sign
   						decoded_signals ->op_mod = 0x2 ;
   						printf("fsgnjx.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   						break;

   				}

   				break;
			case 0x14: // min and max
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = INST_FPU_MISC ;
   				switch (func3) {
   					case 0x0://fmin  .. f[rd] = min of rs1_data and rs2_data
   						decoded_signals ->op_mod = 0x3 ;
   						printf("fmin.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   						break;
   					case 0x1:////fmax  .. f[rd] = max of rs1_data and rs2_data
   						decoded_signals ->op_mod = 0x4 ;
   						printf("fmax.s %s , %s ,%s \n",f_rd_name,f_rs1_name,f_rs2_name);
   						break;
   					}
   				break;
			case 0x60: // conversions to integer
   				decoded_signals -> rd = rd; //** integer register
			   	decoded_signals -> rs1 = rs1+32 ;
			   	if(getBit(fetched_instruction.instr,20)) { //  rs2==1 .. x[rd] = unsigned convert f[rs1]
   					decoded_signals ->op_type = INST_FPU_CVTWUS ;
   					printf("fcvt.wu.s %s , %s  \n",rd_name,f_rs1_name);
   				}else{
   					decoded_signals ->op_type = INST_FPU_CVTWS ;
   					printf("fcvt.w.s %s , %s  \n",rd_name,f_rs1_name);
   				}

   				break;

			case 0x70: // class and move to integer register
   				decoded_signals -> rd = rd; //** integer register
			   	decoded_signals -> rs1 = rs1+32 ;

			   	if(getBit(fetched_instruction.instr,12)) { //  func3==1 .. x[rd] = class of f[rs1]
   					decoded_signals ->op_type = INST_FPU_CLASS ;
   					printf("fclass.s %s , %s  \n",rd_name,f_rs1_name);
   				}else{ // move
   					decoded_signals ->op_type = INST_FPU_MISC ;
   					decoded_signals ->op_mod = 5 ;
   					printf("fmv.x.w %s , %s  \n",rd_name,f_rs1_name);
   				}

   				break;

   			case 0x50: // Compare
   				decoded_signals -> rd = rd ;
			   	decoded_signals -> rs1 = rs1+32 ;
			   	decoded_signals -> rs2 = rs2+32 ;

   				decoded_signals ->op_type = INST_FPU_CMP ;
   				switch (func3) {
   					case 0x0:
   						printf("fle.s %s , %s ,%s \n",rd_name,f_rs1_name,f_rs2_name);
   						break;
   					case 0x1:
   						printf("flt.s %s , %s ,%s \n",rd_name,f_rs1_name,f_rs2_name);
   						break;

   					case 0x2:
   						printf("feq.s %s , %s ,%s \n",rd_name,f_rs1_name,f_rs2_name);
   						break;
   					}
   			case 0x68: // conversions to float
   				decoded_signals -> rd = rd+32;
			   	decoded_signals -> rs1 = rs1 ;
			   	if(getBit(fetched_instruction.instr,20)) { //  rs2==1 .. x[rd] = unsigned convert f[rs1]
   					decoded_signals ->op_type = INST_FPU_CVTWUS ;
   					printf("fcvt.s.wu %s , %s  \n",f_rd_name,rs1_name);
   				}else{
   					decoded_signals ->op_type = INST_FPU_CVTSW ;
   					printf("fcvt.s.w %s , %s  \n",f_rd_name,rs1_name);
   				}

   				break;
   			case 0x78: // move to floating point register
   				decoded_signals -> rd = rd+32 ;
			   	decoded_signals -> rs1 = rs1 ;

   				decoded_signals ->op_type = INST_FPU_MISC ;
   				decoded_signals ->op_mod = 6 ;
   				printf("fmv.w.x %s , %s ,%s  \n",f_rd_name,f_rs1_name,f_rs2_name);
   				break;
   		}
   	break ;
   //****************************	 INST_GPGPU instr 	*****************************//
   case INST_GPGPU:
   	decoded_signals -> ex_type = EX_GPU;
   	switch(func3) {
   		case 0x0:
		   	stalled_warps -> stalled = 1;
	   	   	decoded_signals -> rs1 = rs1;
		   	if (getBit(rs2,0)) {
		   		decoded_signals -> op_type =INST_GPU_PRED;
   				printf("vx_pred %s \n",rs1_name);
		   	} else {
		   		decoded_signals -> op_type =INST_GPU_TMC;
   				printf("vx_tmc %s \n",rs1_name);
		   	}
   			break;
   		case 0x1: //
	   	   	decoded_signals -> ex_type = INST_GPU_WSPAWN;
	   	   	decoded_signals -> rs1 = rs1;
	   	   	decoded_signals -> rs2 = rs2;

	   	   	printf("vx_wspawn %s ,%s \n",rs1_name,rs2_name);

   			break;
   		case 0x2:

   			decoded_signals -> ex_type = INST_GPU_SPLIT;
	   	   	decoded_signals -> rs1 = rs1;
		   	stalled_warps -> stalled = 1;

	   	   	printf("vx_split %s  \n",rs1_name);
   			break;
   		case 0x3:

   			decoded_signals -> ex_type = INST_GPU_JOIN;

	   	   	join_inst -> valid = 1;

	   	   	printf("vx_join \n");
   			break;

   		case 0x4:
   			decoded_signals -> ex_type = INST_GPU_BAR;
	   	   	decoded_signals -> rs1 = rs1;
	   	   	decoded_signals -> rs2 = rs2;

		   	stalled_warps -> stalled = 1;

	   	   	printf("vx_bar %s, %s  \n",rs1_name,rs2_name);
   			break;
   		case 0x5://********************
   			decoded_signals -> ex_type = EX_LSU;
	   	   	decoded_signals -> rs1 = rs1;
	   	   	decoded_signals -> op_mod = 2;

	   	   	decoded_signals -> op_type = INST_LSU_LW;

	   	   	printf("vx_LL  %s  \n",rs1_name);
   			break;

   	}
   	break ;
   //****************************	 INST_GPU instr 	*****************************//
   case INST_GPU:
   	switch(func3) {
   		case 0x0:
   			decoded_signals -> ex_type = EX_GPU;
   			decoded_signals -> op_mod = func2;

	   	   	decoded_signals -> op_type = INST_GPU_TEX;

   			decoded_signals -> rd = rd ;
		   	decoded_signals -> rs1 = rs1 ;
		   	decoded_signals -> rs2 = rs2 ;
		   	decoded_signals -> rs3 = rs3 ;
   	}
   	break ;


   default :
		printf("NOP");
	}

}

