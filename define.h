#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "common.h"
#include "config.h"

///////////////////////////////////////////////////////////////////////////////
#define int_log2(x) 	(uint32_t)(log2(x))
#define NW_BITS         int_log2(NUM_WARPS)

#define NT_BITS         int_log2(NUM_THREADS)

#define NC_BITS         int_log2(NUM_CORES)

#define NB_BITS         int_log2(NUM_BARRIERS)

#define NUM_IREGS       32

#define NRI_BITS        int_log2(NUM_IREGS)

#define NTEX_BITS       int_log2(NUM_TEX_UNITS)

#ifdef EXT_F_ENABLE
#define NUM_REGS        (2 * NUM_IREGS)
#else
#define NUM_REGS        NUM_IREGS
#endif

#define NR_BITS         int_log2(NUM_REGS)

#define CSR_ADDR_BITS   12

#define CSR_WIDTH       12

#define PERF_CTR_BITS   44

#define UUID_BITS       44

///////////////////////////////////////////////////////////////////////////////

#define EX_NOP          0x0
#define EX_ALU          0x1
#define EX_LSU          0x2
#define EX_CSR          0x3
#define EX_FPU          0x4
#define EX_GPU          0x5
#define EX_BITS         3

///////////////////////////////////////////////////////////////////////////////

#define INST_LUI        0b0110111
#define INST_AUIPC      0b0010111
#define INST_JAL        0b1101111
#define INST_JALR       0b1100111
#define INST_B          0b1100011 // branch instructions
#define INST_L          0b0000011 // load instructions
#define INST_S          0b0100011 // store instructions
#define INST_I          0b0010011 // immediate instructions
#define INST_R          0b0110011 // register instructions
#define INST_FENCE      0b0001111 // Fence instructions
#define INST_SYS        0b1110011 // system instructions

#define INST_FL         0b0000111 // float load instruction
#define INST_FS         0b0100111 // float store  instruction
#define INST_FMADD      0b1000011
#define INST_FMSUB      0b1000111
#define INST_FNMSUB     0b1001011
#define INST_FNMADD     0b1001111
#define INST_FCI        0b1010011 // float common instructions

#define INST_GPGPU      0b1101011
#define INST_GPU        0b1011011

#define INST_TEX       0b0101011

///////////////////////////////////////////////////////////////////////////////

#define INST_FRM_RNE    0b000  // round to nearest even
#define INST_FRM_RTZ    0b001  // round to zero
#define INST_FRM_RDN    0b010  // round to -inf
#define INST_FRM_RUP    0b011  // round to +inf
#define INST_FRM_RMM    0b100  // round to nearest max magnitude
#define INST_FRM_DYN    0b111  // dynamic mode
#define INST_FRM_BITS   3

///////////////////////////////////////////////////////////////////////////////

#define INST_OP_BITS    4
#define INST_MOD_BITS   3

///////////////////////////////////////////////////////////////////////////////

#define INST_ALU_ADD         0b0000
#define INST_ALU_LUI         0b0010
#define INST_ALU_AUIPC       0b0011
#define INST_ALU_SLTU        0b0100
#define INST_ALU_SLT         0b0101
#define INST_ALU_SRL         0b1000
#define INST_ALU_SRA         0b1001
#define INST_ALU_SUB         0b1011
#define INST_ALU_AND         0b1100
#define INST_ALU_OR          0b1101
#define INST_ALU_XOR         0b1110
#define INST_ALU_SLL         0b1111
#define INST_ALU_OTHER       0b0111
#define INST_ALU_BITS        4

#define INST_ALU_OP(x)       getBits(x,INST_ALU_BITS-1,0)
#define INST_ALU_OP_CLASS(x) getBits(x,3,2)
#define INST_ALU_SIGNED(x)   getBit(x,0)
#define INST_ALU_IS_BR(x)    getBit(x,0)
#define INST_ALU_IS_MUL(x)   getBit(x,1)


#define INST_BR_EQ           0b0000
#define INST_BR_NE           0b0010
#define INST_BR_LTU          0b0100
#define INST_BR_GEU          0b0110
#define INST_BR_LT           0b0101
#define INST_BR_GE           0b0111
#define INST_BR_JAL          0b1000
#define INST_BR_JALR         0b1001
#define INST_BR_ECALL        0b1010
#define INST_BR_EBREAK       0b1011
#define INST_BR_URET         0b1100
#define INST_BR_SRET         0b1101
#define INST_BR_MRET         0b1110
#define INST_BR_OTHER        0b1111
#define INST_BR_BITS         4

#define INST_BR_NEG(x)       getBit(x,1)
#define INST_BR_LESS(x)      getBit(x,2)
#define INST_BR_STATIC(x)    getBit(x,3)


#define INST_MUL_MUL         0x0
#define INST_MUL_MULH        0x1
#define INST_MUL_MULHSU      0x2
#define INST_MUL_MULHU       0x3
#define INST_MUL_DIV         0x4
#define INST_MUL_DIVU        0x5
#define INST_MUL_REM         0x6
#define INST_MUL_REMU        0x7
#define INST_MUL_BITS        3

#define INST_MUL_IS_DIV(x)   getBit(x,2)

#define INST_FMT_B           0b000
#define INST_FMT_H           0b001
#define INST_FMT_W           0b010
#define INST_FMT_BU          0b100
#define INST_FMT_HU          0b101

#define INST_LSU_LB          0b0000
#define INST_LSU_LH          0b0001
#define INST_LSU_LW          0b0010
#define INST_LSU_LBU         0b0100
#define INST_LSU_LHU         0b0101
#define INST_LSU_SB          0b1000
#define INST_LSU_SH          0b1001
#define INST_LSU_SW          0b1010
#define INST_LSU_BITS        4

#define INST_LSU_FMT(x)      getBits(x,2,0)
#define INST_LSU_WSIZE(x)    getBits(x,1,0)


#define INST_LSU_IS_MEM(x)   ((x==0) ? true : false)
#define INST_LSU_IS_FENCE(x)   ((x==1) ? true : false)
#define INST_LSU_IS_PREFETCH(x)   ((x==2) ? true : false)


#define INST_FENCE_BITS      1
#define INST_FENCE_D         0x0
#define INST_FENCE_I         0x1

#define INST_CSR_RW          0x1
#define INST_CSR_RS          0x2
#define INST_CSR_RC          0x3
#define INST_CSR_OTHER       0x0
#define INST_CSR_BITS        2

#define INST_FPU_ADD         0x0
#define INST_FPU_SUB         0x4
#define INST_FPU_MUL         0x8
#define INST_FPU_DIV         0xC
#define INST_FPU_CVTWS       0x1  // FCVT.W.S
#define INST_FPU_CVTWUS      0x5  // FCVT.WU.S
#define INST_FPU_CVTSW       0x9  // FCVT.S.W
#define INST_FPU_CVTSWU      0xD  // FCVT.S.WU
#define INST_FPU_SQRT        0x2
#define INST_FPU_CLASS       0x6
#define INST_FPU_CMP         0xA
#define INST_FPU_MISC        0xE  // SGNJ, SGNJN, SGNJX, FMIN, FMAX, MVXW, MVWX
#define INST_FPU_MADD        0x3
#define INST_FPU_MSUB        0x7
#define INST_FPU_NMSUB       0xB
#define INST_FPU_NMADD       0xF
#define INST_FPU_BITS        4

#define INST_GPU_TMC         0x0
#define INST_GPU_WSPAWN      0x1
#define INST_GPU_SPLIT       0x2
#define INST_GPU_JOIN        0x3
#define INST_GPU_BAR         0x4
#define INST_GPU_PRED        0x5
#define INST_GPU_TEX         0x6
#define INST_GPU_BITS        4

///////////////////////////////////////////////////////////////////////////////

#ifdef EXT_M_ENABLE
    #define ISA_EXT_M   (1 << 12)
#else
    #define ISA_EXT_M   0
#endif

#ifdef EXT_F_ENABLE
    #define ISA_EXT_F   (1 << 5)
#else
    #define ISA_EXT_F   0
#endif

//#define ISA_CODE  (0 <<  0) // A - Atomic Instructions extension \
//                | (0 <<  1) // B - Tentatively reserved for Bit operations extension  \
//                | (0 <<  2) // C - Compressed extension \
//                | (0 <<  3) // D - Double precsision floating-point extension \
//                | (0 <<  4) // E - RV32E base ISA \
//                |ISA_EXT_F // F - Single precsision floating-point extension \
//                | (0 <<  6) // G - Additional standard extensions present \
//                | (0 <<  7) // H - Hypervisor mode implemented \
//                | (1 <<  8) // I - RV32I/64I/128I base ISA \
//                | (0 <<  9) // J - Reserved \
//                | (0 << 10) // K - Reserved \
//                | (0 << 11) // L - Tentatively reserved for Bit operations extension \
//                |ISA_EXT_M // M - Integer Multiply/Divide extension \
//                | (0 << 13) // N - User level interrupts supported \
//                | (0 << 14) // O - Reserved \
//                | (0 << 15) // P - Tentatively reserved for Packed-SIMD extension \
//                | (0 << 16) // Q - Quad-precision floating-point extension \
//                | (0 << 17) // R - Reserved \
//                | (0 << 18) // S - Supervisor mode implemented \
//                | (0 << 19) // T - Tentatively reserved for Transactional Memory extension \
//                | (1 << 20) // U - User mode implemented \
//                | (0 << 21) // V - Tentatively reserved for Vector extension \
//                | (0 << 22) // W - Reserved \
//                | (1 << 23) // X - Non-standard extensions present \
//                | (0 << 24) // Y - Reserved \
//                | (0 << 25) // Z - Reserved
//
///////////////////////////////////////////////////////////////////////////////

// non-cacheable tag bits
#define NC_TAG_BIT              1

// texture tag bits
#define TEX_TAG_BIT             1

// cache address type bits
#define CACHE_ADDR_TYPE_BITS    (NC_TAG_BIT + SM_ENABLE)

////////////////////////// Icache Configurable Knobs //////////////////////////

// Cache ID
#define ICACHE_ID             ((uint32_t)L3_ENABLE + (uint32_t)L2_ENABLE * NUM_CLUSTERS + CORE_ID * 3 + 0)

// Word size in bytes
#define ICACHE_WORD_SIZE        4

// Block size in bytes
#define ICACHE_LINE_SIZE        L1_BLOCK_SIZE

// TAG sharing enable
#define ICACHE_CORE_TAG_ID_BITS NW_BITS

// Core request tag bits
#define ICACHE_CORE_TAG_WIDTH   (UUID_BITS + ICACHE_CORE_TAG_ID_BITS)

// Memory request data bits
#define ICACHE_MEM_DATA_WIDTH   (ICACHE_LINE_SIZE * 8)

// Memory request address bits
#define ICACHE_MEM_ADDR_WIDTH   (32 - int_log2(ICACHE_LINE_SIZE))

// Memory request tag bits
#define ICACHE_MEM_TAG_WIDTH    int_log2(ICACHE_MSHR_SIZE)

////////////////////////// Dcache Configurable Knobs //////////////////////////

// Cache ID
#define DCACHE_ID             ((uint32_t)L3_ENABLE + (uint32_t)L2_ENABLE * NUM_CLUSTERS + CORE_ID * 3 + 1)

// Word size in bytes
#define DCACHE_WORD_SIZE        4

// Block size in bytes
#define DCACHE_LINE_SIZE        L1_BLOCK_SIZE

// Core request tag bits
#define LSUQ_ADDR_BITS          int_log2(LSUQ_SIZE)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifdef EXT_TEX_ENABLE
#define LSU_TAG_ID_BITS         MAX(LSUQ_ADDR_BITS, 2)
#define LSU_TEX_DCACHE_TAG_BITS (UUID_BITS + LSU_TAG_ID_BITS + CACHE_ADDR_TYPE_BITS)
#define DCACHE_CORE_TAG_ID_BITS (LSU_TAG_ID_BITS + CACHE_ADDR_TYPE_BITS + TEX_TAG_BIT)
#else
#define LSU_TAG_ID_BITS         LSUQ_ADDR_BITS
#define DCACHE_CORE_TAG_ID_BITS (LSU_TAG_ID_BITS + CACHE_ADDR_TYPE_BITS)
#endif
#define DCACHE_CORE_TAG_WIDTH   (UUID_BITS + DCACHE_CORE_TAG_ID_BITS)

// Memory request data bits
#define DCACHE_MEM_DATA_WIDTH   (DCACHE_LINE_SIZE * 8)

// Memory request address bits
#define DCACHE_MEM_ADDR_WIDTH   (32 - int_log2(DCACHE_LINE_SIZE))

// Memory byte enable bits
#define DCACHE_MEM_BYTEEN_WIDTH DCACHE_LINE_SIZE

// Input request size
#define DCACHE_NUM_REQS         NUM_THREADS

// Memory request tag bits
#define _DMEM_ADDR_RATIO_W      int_log2(DCACHE_LINE_SIZE / DCACHE_WORD_SIZE)
#define _DNC_MEM_TAG_WIDTH      (int_log2(DCACHE_NUM_REQS) + _DMEM_ADDR_RATIO_W + DCACHE_CORE_TAG_WIDTH)
#define DCACHE_MEM_TAG_WIDTH    MAX((int_log2(DCACHE_NUM_BANKS) + int_log2(DCACHE_MSHR_SIZE) + NC_TAG_BIT), _DNC_MEM_TAG_WIDTH)

// Merged D-cache/I-cache memory tag
#define L1_MEM_TAG_WIDTH        (MAX(ICACHE_MEM_TAG_WIDTH, DCACHE_MEM_TAG_WIDTH) + int_log2(2))

////////////////////////// SM Configurable Knobs //////////////////////////////

// Cache ID
#define SMEM_ID                 ((uint32_t)L3_ENABLE + (uint32_t)L2_ENABLE * NUM_CLUSTERS + CORE_ID * 3 + 2)

// Word size in bytes
#define SMEM_WORD_SIZE          4

// bank address offset
#define SMEM_BANK_ADDR_OFFSET   int_log2(STACK_SIZE / SMEM_WORD_SIZE)

// Input request size
#define SMEM_NUM_REQS           NUM_THREADS

////////////////////////// L2cache Configurable Knobs /////////////////////////

// Cache ID
#define L2_CACHE_ID              ((uint32_t)L3_ENABLE + CLUSTER_ID)

// Word size in bytes
#define L2_WORD_SIZE             DCACHE_LINE_SIZE

// Block size in bytes
#define L2_CACHE_LINE_SIZE       ((L2_ENABLE) ? MEM_BLOCK_SIZE : L2_WORD_SIZE)

// Input request tag bits
#define L2_CORE_TAG_WIDTH        (DCACHE_CORE_TAG_WIDTH + int_log2(NUM_CORES))

// Memory request data bits
#define L2_MEM_DATA_WIDTH        (L2_CACHE_LINE_SIZE * 8)

// Memory request address bits
#define L2_MEM_ADDR_WIDTH        (32 - int_log2(L2_CACHE_LINE_SIZE))

// Memory byte enable bits
#define L2_MEM_BYTEEN_WIDTH      L2_CACHE_LINE_SIZE

// Input request size
#define L2_NUM_REQS              NUM_CORES

// Memory request tag bits
#define _L2_MEM_ADDR_RATIO_W     int_log2(L2_CACHE_LINE_SIZE / L2_WORD_SIZE)
#define _L2_NC_MEM_TAG_WIDTH     (int_log2(L2_NUM_REQS) + _L2_MEM_ADDR_RATIO_W + L1_MEM_TAG_WIDTH)
#define _L2_MEM_TAG_WIDTH        MAX((int_log2(L2_NUM_BANKS) + int_log2(L2_MSHR_SIZE) + NC_TAG_BIT), _L2_NC_MEM_TAG_WIDTH)
#define L2_MEM_TAG_WIDTH         ((L2_ENABLE) ? _L2_MEM_TAG_WIDTH : (L1_MEM_TAG_WIDTH + int_log2(L2_NUM_REQS)))

////////////////////////// L3cache Configurable Knobs /////////////////////////

// Cache ID
#define L3_CACHE_ID              0

// Word size in bytes
#define L3_WORD_SIZE             L2_CACHE_LINE_SIZE

// Block size in bytes
#define L3_CACHE_LINE_SIZE       ((L3_ENABLE) ? MEM_BLOCK_SIZE : L3_WORD_SIZE)

// Input request tag bits
#define L3_CORE_TAG_WIDTH        (L2_CORE_TAG_WIDTH + int_log2(NUM_CLUSTERS))

// Memory request data bits
#define L3_MEM_DATA_WIDTH        (L3_CACHE_LINE_SIZE * 8)

// Memory request address bits
#define L3_MEM_ADDR_WIDTH        (32 - int_log2(L3_CACHE_LINE_SIZE))

// Memory byte enable bits
#define L3_MEM_BYTEEN_WIDTH      L3_CACHE_LINE_SIZE

// Input request size
#define L3_NUM_REQS              NUM_CLUSTERS

// Memory request tag bits
#define _L3_MEM_ADDR_RATIO_W     int_log2(L3_CACHE_LINE_SIZE / L3_WORD_SIZE)
#define _L3_NC_MEM_TAG_WIDTH     (int_log2(L3_NUM_REQS) + _L3_MEM_ADDR_RATIO_W + L2_MEM_TAG_WIDTH)
#define _L3_MEM_TAG_WIDTH        MAX((int_log2(L3_NUM_BANKS) + int_log2(L3_MSHR_SIZE) + NC_TAG_BIT), _L3_NC_MEM_TAG_WIDTH)
#define L3_MEM_TAG_WIDTH         ((L3_ENABLE) ? _L3_MEM_TAG_WIDTH : (L2_MEM_TAG_WIDTH + int_log2(L3_NUM_REQS)))

///////////////////////////////////////////////////////////////////////////////

#define VX_MEM_BYTEEN_WIDTH     L3_MEM_BYTEEN_WIDTH
#define VX_MEM_ADDR_WIDTH       L3_MEM_ADDR_WIDTH
#define VX_MEM_DATA_WIDTH       L3_MEM_DATA_WIDTH
#define VX_MEM_TAG_WIDTH        L3_MEM_TAG_WIDTH
#define VX_CORE_TAG_WIDTH       L3_CORE_TAG_WIDTH
#define VX_CSR_ID_WIDTH         int_log2(NUM_CLUSTERS * NUM_CORES)

#define  TO_FULL_ADDR(x) concatenate ( x ,  (unsigned int)int_log2(number)+1, 0 ,  32-((unsigned int)int_log2(number)+1)) ;

///////////////////////////////////////////////////////////////////////////////

