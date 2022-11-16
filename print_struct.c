#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "define.h"
#include "config.h"
#include "common.h"
#include "issue_stage.h"

void printf_decode(VX_decode decode)
{
    printf("decode.uuid=%"PRIu64"\n",decode.uuid);
    printf("decode.wid=%d\n",decode.wid);
    printf("decode.tmask=%d\n",decode.tmask);
    printf("decode.PC=%d\n",decode.PC);
    printf("decode.ex_type=%d\n",decode.ex_type);
    printf("decode.op_type=%d\n",decode.op_type);
    printf("decode.op_mod=%d\n",decode.op_mod);
    printf("decode.op_mod=%d\n",decode.op_mod);
    printf("decode.imm=%d\n",decode.imm);
    printf("decode.rd=%d\n",decode.rd);
    printf("decode.rs1=%d\n",decode.rs1);
    printf("decode.rs2=%d\n",decode.rs2);
    printf("decode.rs3=%d\n",decode.rs3);
    printf("decode.wb=%d\n",decode.wb);
    printf("decode.use_PC=%d\n",decode.use_PC);
    printf("decode.use_imm=%d\n",decode.use_imm);

}

void printf_writeback(VX_writeback writeback)
{
    printf("writeback.uuid=%"PRIu64"\n",writeback.uuid);
    printf("writeback.wid=%d\n",writeback.wid);
    printf("writeback.tmask=%d\n",writeback.tmask);
    printf("writeback.PC=%d\n",writeback.PC);
    printf("writeback.rd=%d\n",writeback.rd);
    for ( int i = 0; i <NUM_THREADS; i++)
    printf("writeback.data[%d]=%d\n",i,writeback.data[i]);
    printf("writeback.eop=%d\n",writeback.eop);

}

void printf_ibuffer(VX_decode ibuffer)
{
    printf("ibuffer.uuid=%"PRIu64"\n",ibuffer.uuid);
    printf("ibuffer.wid=%d\n",ibuffer.wid);
    printf("ibuffer.tmask=%d\n",ibuffer.tmask);
    printf("ibuffer.PC=%d\n",ibuffer.PC);
    printf("ibuffer.ex_type=%d\n",ibuffer.ex_type);
    printf("ibuffer.op_type=%d\n",ibuffer.op_type);
    printf("ibuffer.op_mod=%d\n",ibuffer.op_mod);
    printf("ibuffer.imm=%d\n",ibuffer.imm);
    printf("ibuffer.rd=%d\n",ibuffer.rd);
    printf("ibuffer.rs1=%d\n",ibuffer.rs1);
    printf("ibuffer.rs2=%d\n",ibuffer.rs2);
    printf("ibuffer.rs3=%d\n",ibuffer.rs3);
    printf("ibuffer.wb=%d\n",ibuffer.wb);
    printf("ibuffer.use_PC=%d\n",ibuffer.use_PC);
    printf("ibuffer.use_imm=%d\n",ibuffer.use_imm);

}
void printf_gpr_rsp(VX_gpr_rsp gpr_rsp)
{
    for ( int i = 0; i <NUM_THREADS; i++)
    {
    printf("gpr_rsp.rs1_data[%d]=%d\n",i,gpr_rsp.rs1_data[i]);
    printf("gpr_rsp.rs2_data[%d]=%d\n",i,gpr_rsp.rs2_data[i]);
    printf("gpr_rsp.rs3_data[%d]=%d\n",i,gpr_rsp.rs3_data[i]);
    }

}

void printf_gpr_req(VX_gpr_req gpr_req)
{
    printf("gpr_req.wid=%d\n",gpr_req.wid);
    printf("gpr_req.rs1=%d\n",gpr_req.rs1);
    printf("gpr_req.rs2=%d\n",gpr_req.rs2);
    printf("gpr_req.rs3=%d\n",gpr_req.rs3);

}

void printf_alu_req(VX_alu_req alu_req)
{
    printf("alu_req.uuid=%"PRIu64"\n",alu_req.uuid);
    printf("alu_req.wid=%d\n",alu_req.wid);
    printf("alu_req.tmask=%d\n",alu_req.tmask);
    printf("alu_req.PC=%d\n",alu_req.PC);
    printf("alu_req.next_PC=%d\n",alu_req.next_PC);
    printf("alu_req.op_type=%d\n",alu_req.op_type);
    printf("alu_req.op_mod=%d\n",alu_req.op_mod);
    printf("alu_req.use_PC=%d\n",alu_req.use_PC);
    printf("alu_req.use_imm=%d\n",alu_req.use_imm);
    printf("alu_req.imm=%d\n",alu_req.imm);
    printf("alu_req.tid=%d\n",alu_req.tid);
    for ( int i = 0; i <NUM_THREADS; i++)
    {
    printf("alu_req.rs1_data[%d]=%d\n",i,alu_req.rs1_data[i]);
    printf("alu_req.rs2_data[%d]=%d\n",i,alu_req.rs2_data[i]);
    }
    printf("alu_req.rd=%d\n",alu_req.rd);
    printf("alu_req.wb=%d\n",alu_req.wb);
}

void printf_lsu_req(VX_lsu_req lsu_req)
{
    printf("lsu_req.uuid=%"PRIu64"\n",lsu_req.uuid);
    printf("lsu_req.wid=%d\n",lsu_req.wid);
    printf("lsu_req.tmask=%d\n",lsu_req.tmask);
    printf("lsu_req.PC=%d\n",lsu_req.PC);
    printf("lsu_req.offset=%d\n",lsu_req.offset);
    printf("lsu_req.op_type=%d\n",lsu_req.op_type);
    printf("lsu_req.is_fence=%d\n",lsu_req.is_fence);
    printf("lsu_req.is_prefetch=%d\n",lsu_req.is_prefetch);
    for ( int i = 0; i <NUM_THREADS; i++)
    {
    printf("lsu_req.store_data[%d]=%d\n",i,lsu_req.store_data[i]);
    printf("lsu_req.base_addr[%d]=%d\n",i,lsu_req.base_addr[i]);
    }
    printf("lsu_req.rd=%d\n",lsu_req.rd);
    printf("lsu_req.wb=%d\n",lsu_req.wb);
}

void printf_csr_req(VX_csr_req csr_req)
{
    printf("csr_req.uuid=%"PRIu64"\n",csr_req.uuid);
    printf("csr_req.wid=%d\n",csr_req.wid);
    printf("csr_req.tmask=%d\n",csr_req.tmask);
    printf("csr_req.PC=%d\n",csr_req.PC);
    printf("csr_req.op_type=%d\n",csr_req.op_type);
    printf("csr_req.addr=%d\n",csr_req.addr);
    printf("csr_req.use_imm=%d\n",csr_req.use_imm);
    printf("csr_req.imm=%d\n",csr_req.imm);
    printf("csr_req.rs1_data=%d\n",csr_req.rs1_data);
    printf("csr_req.rd=%d\n",csr_req.rd);
    printf("csr_req.wb=%d\n",csr_req.wb);
}

void printf_fpu_req(VX_fpu_req fpu_req)
{
    printf("fpu_req.uuid=%"PRIu64"\n",fpu_req.uuid);
    printf("fpu_req.wid=%d\n",fpu_req.wid);
    printf("fpu_req.tmask=%d\n",fpu_req.tmask);
    printf("fpu_req.PC=%d\n",fpu_req.PC);
    printf("fpu_req.op_type=%d\n",fpu_req.op_type);
    printf("fpu_req.op_mod=%d\n",fpu_req.op_mod);
    for ( int i = 0; i <NUM_THREADS; i++)
    {
    printf("fpu_req.rs1_data[%d]=%d\n",i,fpu_req.rs1_data[i]);
    printf("fpu_req.rs2_data[%d]=%d\n",i,fpu_req.rs2_data[i]);
    printf("fpu_req.rs3_data[%d]=%d\n",i,fpu_req.rs3_data[i]);
    }
    printf("fpu_req.rd=%d\n",fpu_req.rd);
    printf("fpu_req.wb=%d\n",fpu_req.wb);
}

void printf_gpu_req(VX_gpu_req gpu_req)
{
    printf("gpu_req.uuid=%"PRIu64"\n",gpu_req.uuid);
    printf("gpu_req.wid=%d\n",gpu_req.wid);
    printf("gpu_req.tmask=%d\n",gpu_req.tmask);
    printf("gpu_req.PC=%d\n",gpu_req.PC);
    printf("gpu_req.next_PC=%d\n",gpu_req.next_PC);
    printf("gpu_req.op_type=%d\n",gpu_req.op_type);
    printf("gpu_req.op_mod=%d\n",gpu_req.op_mod);
    printf("gpu_req.tid=%d\n",gpu_req.tid);
    for ( int i = 0; i <NUM_THREADS; i++)
    {
    printf("gpu_req.rs1_data[%d]=%d\n",i,gpu_req.rs1_data[i]);
    printf("gpu_req.rs2_data[%d]=%d\n",i,gpu_req.rs2_data[i]);
    }
    printf("gpu_req.rd=%d\n",gpu_req.rd);
    printf("gpu_req.wb=%d\n",gpu_req.wb);
}
