#include <stdio.h>
#include <math.h>
#include <stdint.h>


#ifndef XLEN
#define XLEN 32
#endif

#ifndef NUM_CLUSTERS
#define NUM_CLUSTERS 1
#endif

#ifndef NUM_CORES
#define NUM_CORES 1
#endif

#ifndef NUM_WARPS
#define NUM_WARPS 4
#endif

#ifndef NUM_THREADS
#define NUM_THREADS 4
#endif

#ifndef NUM_BARRIERS
#define NUM_BARRIERS 4
#endif

#ifndef L2_ENABLE
#define L2_ENABLE 0
#endif

#ifndef L3_ENABLE
#define L3_ENABLE 0
#endif

#ifndef SM_ENABLE
#define SM_ENABLE 1
#endif

#ifndef MEM_BLOCK_SIZE
#define MEM_BLOCK_SIZE 64
#endif

#ifndef L1_BLOCK_SIZE
#define L1_BLOCK_SIZE ((L2_ENABLE+L3_ENABLE>0) ? 16 : MEM_BLOCK_SIZE)
#endif

#ifndef STARTUP_ADDR
#define STARTUP_ADDR 0x80000000
#endif

#ifndef IO_BASE_ADDR
#define IO_BASE_ADDR 0xFF000000
#endif

#ifndef IO_ADDR_SIZE
#define IO_ADDR_SIZE (0xFFFFFFFF - IO_BASE_ADDR + 1)
#endif

#ifndef IO_COUT_ADDR
#define IO_COUT_ADDR (0xFFFFFFFF - MEM_BLOCK_SIZE + 1)
#endif

#ifndef IO_COUT_SIZE
#define IO_COUT_SIZE MEM_BLOCK_SIZE
#endif

#ifndef IO_CSR_ADDR
#define IO_CSR_ADDR IO_BASE_ADDR
#endif

#ifndef SMEM_BASE_ADDR
#define SMEM_BASE_ADDR IO_BASE_ADDR
#endif

#ifndef EXT_M_DISABLE
#define EXT_M_ENABLE
#endif

#ifndef EXT_F_DISABLE
#define EXT_F_ENABLE
#endif

// Device identification
#define VENDOR_ID           0
#define ARCHITECTURE_ID     0
#define IMPLEMENTATION_ID   0

///////////////////////////////////////////////////////////////////////////////

#ifndef LATENCY_IMUL
#define LATENCY_IMUL 3
#endif

#ifndef LATENCY_FNCP
#define LATENCY_FNCP 2
#endif

#ifndef LATENCY_FMA
#define LATENCY_FMA 4
#endif

#ifndef LATENCY_FDIV
#ifdef ALTERA_S10
#define LATENCY_FDIV 34
else
#define LATENCY_FDIV 15
#endif
#endif

#ifndef LATENCY_FSQRT
#ifdef ALTERA_S10
#define LATENCY_FSQRT 25
else
#define LATENCY_FSQRT 10
#endif
#endif

#ifndef LATENCY_FDIVSQRT
#define LATENCY_FDIVSQRT 32
#endif

#ifndef LATENCY_FCVT
#define LATENCY_FCVT 5
#endif

#define RESET_DELAY 6

// CSR Addresses //////////////////////////////////////////////////////////////

// User Floating-Point CSRs
#define CSR_FFLAGS      0x001
#define CSR_FRM         0x002
#define CSR_FCSR        0x003

#define CSR_SATP        0x180

#define CSR_PMPCFG0     0x3A0
#define CSR_PMPADDR0    0x3B0

#define CSR_MSTATUS     0x300
#define CSR_MISA        0x301
#define CSR_MEDELEG     0x302
#define CSR_MIDELEG     0x303
#define CSR_MIE         0x304
#define CSR_MTVEC       0x305

#define CSR_MEPC        0x341

// Machine Performance-monitoring counters
#define CSR_MPM_BASE                0xB00
#define CSR_MPM_BASE_H              0xB80
// PERF: pipeline
#define CSR_MCYCLE                  0xB00
#define CSR_MCYCLE_H                0xB80
#define CSR_MPM_RESERVED            0xB01
#define CSR_MPM_RESERVED_H          0xB81
#define CSR_MINSTRET                0xB02
#define CSR_MINSTRET_H              0xB82
#define CSR_MPM_IBUF_ST             0xB03
#define CSR_MPM_IBUF_ST_H           0xB83
#define CSR_MPM_SCRB_ST             0xB04
#define CSR_MPM_SCRB_ST_H           0xB84
#define CSR_MPM_ALU_ST              0xB05
#define CSR_MPM_ALU_ST_H            0xB85
#define CSR_MPM_LSU_ST              0xB06
#define CSR_MPM_LSU_ST_H            0xB86
#define CSR_MPM_CSR_ST              0xB07
#define CSR_MPM_CSR_ST_H            0xB87
#define CSR_MPM_FPU_ST              0xB08
#define CSR_MPM_FPU_ST_H            0xB88
#define CSR_MPM_GPU_ST              0xB09
#define CSR_MPM_GPU_ST_H            0xB89
// PERF: decode
#define CSR_MPM_LOADS               0xB0A
#define CSR_MPM_LOADS_H             0xB8A
#define CSR_MPM_STORES              0xB0B
#define CSR_MPM_STORES_H            0xB8B
#define CSR_MPM_BRANCHES            0xB0C
#define CSR_MPM_BRANCHES_H          0xB8C
// PERF: icache
#define CSR_MPM_ICACHE_READS        0xB0D     // total reads
#define CSR_MPM_ICACHE_READS_H      0xB8D
#define CSR_MPM_ICACHE_MISS_R       0xB0E     // read misses
#define CSR_MPM_ICACHE_MISS_R_H     0xB8E
// PERF: dcache
#define CSR_MPM_DCACHE_READS        0xB0F     // total reads
#define CSR_MPM_DCACHE_READS_H      0xB8F
#define CSR_MPM_DCACHE_WRITES       0xB10     // total writes
#define CSR_MPM_DCACHE_WRITES_H     0xB90
#define CSR_MPM_DCACHE_MISS_R       0xB11     // read misses
#define CSR_MPM_DCACHE_MISS_R_H     0xB91
#define CSR_MPM_DCACHE_MISS_W       0xB12     // write misses
#define CSR_MPM_DCACHE_MISS_W_H     0xB92
#define CSR_MPM_DCACHE_BANK_ST      0xB13     // bank conflicts
#define CSR_MPM_DCACHE_BANK_ST_H    0xB93
#define CSR_MPM_DCACHE_MSHR_ST      0xB14     // MSHR stalls
#define CSR_MPM_DCACHE_MSHR_ST_H    0xB94
// PERF: smem
#define CSR_MPM_SMEM_READS          0xB15     // total reads
#define CSR_MPM_SMEM_READS_H        0xB95
#define CSR_MPM_SMEM_WRITES         0xB16     // total writes
#define CSR_MPM_SMEM_WRITES_H       0xB96
#define CSR_MPM_SMEM_BANK_ST        0xB17     // bank conflicts
#define CSR_MPM_SMEM_BANK_ST_H      0xB97
// PERF: memory
#define CSR_MPM_MEM_READS           0xB18     // memory reads
#define CSR_MPM_MEM_READS_H         0xB98
#define CSR_MPM_MEM_WRITES          0xB19     // memory writes
#define CSR_MPM_MEM_WRITES_H        0xB99
#define CSR_MPM_MEM_LAT             0xB1A     // memory latency
#define CSR_MPM_MEM_LAT_H           0xB9A
// PERF: texunit
#define CSR_MPM_TEX_READS           0xB1B     // texture accesses
#define CSR_MPM_TEX_READS_H         0xB9B
#define CSR_MPM_TEX_LAT             0xB1C     // texture latency
#define CSR_MPM_TEX_LAT_H           0xB9C

// Machine Information Registers
#define CSR_MVENDORID   0xF11
#define CSR_MARCHID     0xF12
#define CSR_MIMPID      0xF13
#define CSR_MHARTID     0xF14

// User SIMT CSRs
#define CSR_WTID        0xCC0
#define CSR_LTID        0xCC1
#define CSR_GTID        0xCC2
#define CSR_LWID        0xCC3
#define CSR_GWID        CSR_MHARTID
#define CSR_GCID        0xCC5
#define CSR_TMASK       0xCC4

// Machine SIMT CSRs
#define CSR_NT          0xFC0
#define CSR_NW          0xFC1
#define CSR_NC          0xFC2

////////// Texture Units //////////////////////////////////////////////////////

#define NUM_TEX_UNITS           2
#define TEX_SUBPIXEL_BITS       8

#define TEX_DIM_BITS            15
#define TEX_LOD_MAX             TEX_DIM_BITS
#define TEX_LOD_BITS            4

#define TEX_FXD_BITS            32
#define TEX_FXD_FRAC            (TEX_DIM_BITS+TEX_SUBPIXEL_BITS)

#define TEX_STATE_ADDR          0
#define TEX_STATE_WIDTH         1
#define TEX_STATE_HEIGHT        2
#define TEX_STATE_FORMAT        3
#define TEX_STATE_FILTER        4
#define TEX_STATE_WRAPU         5
#define TEX_STATE_WRAPV         6
#define TEX_STATE_MIPOFF(lod)   (7+(lod))
#define NUM_TEX_STATES          (TEX_STATE_MIPOFF(TEX_LOD_MAX)+1)

#define CSR_TEX_UNIT            0xFD0

#define CSR_TEX_STATE_BEGIN     0xFD1
#define CSR_TEX_ADDR            (CSR_TEX_STATE_BEGIN+TEX_STATE_ADDR)
#define CSR_TEX_WIDTH           (CSR_TEX_STATE_BEGIN+TEX_STATE_WIDTH)
#define CSR_TEX_HEIGHT          (CSR_TEX_STATE_BEGIN+TEX_STATE_HEIGHT)
#define CSR_TEX_FORMAT          (CSR_TEX_STATE_BEGIN+TEX_STATE_FORMAT)
#define CSR_TEX_FILTER          (CSR_TEX_STATE_BEGIN+TEX_STATE_FILTER)
#define CSR_TEX_WRAPU           (CSR_TEX_STATE_BEGIN+TEX_STATE_WRAPU)
#define CSR_TEX_WRAPV           (CSR_TEX_STATE_BEGIN+TEX_STATE_WRAPV)
#define CSR_TEX_MIPOFF(lod)     (CSR_TEX_STATE_BEGIN+TEX_STATE_MIPOFF(lod))
#define CSR_TEX_STATE_END       (CSR_TEX_STATE_BEGIN+NUM_TEX_STATES)

#define CSR_TEX_STATE(addr)     ((addr) - CSR_TEX_STATE_BEGIN)

// Pipeline Queues ////////////////////////////////////////////////////////////

// Size of Instruction Buffer
#ifndef IBUF_SIZE
#define IBUF_SIZE 2
#endif

// Size of LSU Request Queue
#ifndef LSUQ_SIZE
#define LSUQ_SIZE (NUM_WARPS * 2)
#endif

// Size of FPU Request Queue
#ifndef FPUQ_SIZE
#define FPUQ_SIZE 8
#endif

// Texture Unit Request Queue
#ifndef TEXQ_SIZE
#define TEXQ_SIZE (NUM_WARPS * 2)
#endif

// Icache Configurable Knobs //////////////////////////////////////////////////

// Size of cache in bytes
#ifndef ICACHE_SIZE
#define ICACHE_SIZE 16384
#endif

// Core Request Queue Size
#ifndef ICACHE_CREQ_SIZE
#define ICACHE_CREQ_SIZE 0
#endif

// Core Response Queue Size
#ifndef ICACHE_CRSQ_SIZE
#define ICACHE_CRSQ_SIZE 2
#endif

// Miss Handling Register Size
#ifndef ICACHE_MSHR_SIZE
#define ICACHE_MSHR_SIZE NUM_WARPS
#endif

// Memory Request Queue Size
#ifndef ICACHE_MREQ_SIZE
#define ICACHE_MREQ_SIZE 4
#endif

// Memory Response Queue Size
#ifndef ICACHE_MRSQ_SIZE
#define ICACHE_MRSQ_SIZE 0
#endif

// Dcache Configurable Knobs //////////////////////////////////////////////////

// Size of cache in bytes
#ifndef DCACHE_SIZE
#define DCACHE_SIZE 16384
#endif

// Number of banks
#ifndef DCACHE_NUM_BANKS
#define DCACHE_NUM_BANKS NUM_THREADS
#endif

// Number of ports per bank
#ifndef DCACHE_NUM_PORTS
#define DCACHE_NUM_PORTS 1
#endif

// Core Request Queue Size
#ifndef DCACHE_CREQ_SIZE
#define DCACHE_CREQ_SIZE 0
#endif

// Core Response Queue Size
#ifndef DCACHE_CRSQ_SIZE
#define DCACHE_CRSQ_SIZE 2
#endif

// Miss Handling Register Size
#ifndef DCACHE_MSHR_SIZE
#define DCACHE_MSHR_SIZE LSUQ_SIZE
#endif

// Memory Request Queue Size
#ifndef DCACHE_MREQ_SIZE
#define DCACHE_MREQ_SIZE 4
#endif

// Memory Response Queue Size
#ifndef DCACHE_MRSQ_SIZE
#define DCACHE_MRSQ_SIZE 0
#endif

// SM Configurable Knobs //////////////////////////////////////////////////////

// per thread stack size
#ifndef STACK_LOG2_SIZE
#define STACK_LOG2_SIZE 10
#endif
#define STACK_SIZE (1 << STACK_LOG2_SIZE)

// Size of cache in bytes
#ifndef SMEM_SIZE
#define SMEM_SIZE (STACK_SIZE * NUM_WARPS * NUM_THREADS)
#endif

// Number of banks
#ifndef SMEM_NUM_BANKS
#define SMEM_NUM_BANKS NUM_THREADS
#endif

// Core Request Queue Size
#ifndef SMEM_CREQ_SIZE
#define SMEM_CREQ_SIZE 2
#endif

// Core Response Queue Size
#ifndef SMEM_CRSQ_SIZE
#define SMEM_CRSQ_SIZE 2
#endif

// L2cache Configurable Knobs /////////////////////////////////////////////////

// Size of cache in bytes
#ifndef L2_CACHE_SIZE
#define L2_CACHE_SIZE 131072
#endif

// Number of banks
#ifndef L2_NUM_BANKS
#define L2_NUM_BANKS ((NUM_CORES < 4) ? NUM_CORES : 4)
#endif

// Number of ports per bank
#ifndef L2_NUM_PORTS
#define L2_NUM_PORTS 1
#endif

// Core Request Queue Size
#ifndef L2_CREQ_SIZE
#define L2_CREQ_SIZE 0
#endif

// Core Response Queue Size
#ifndef L2_CRSQ_SIZE
#define L2_CRSQ_SIZE 2
#endif

// Miss Handling Register Size
#ifndef L2_MSHR_SIZE
#define L2_MSHR_SIZE 16
#endif

// Memory Request Queue Size
#ifndef L2_MREQ_SIZE
#define L2_MREQ_SIZE 4
#endif

// Memory Response Queue Size
#ifndef L2_MRSQ_SIZE
#define L2_MRSQ_SIZE 0
#endif

// L3cache Configurable Knobs /////////////////////////////////////////////////

// Size of cache in bytes
#ifndef L3_CACHE_SIZE
#define L3_CACHE_SIZE 1048576
#endif

// Number of banks
#ifndef L3_NUM_BANKS
#define L3_NUM_BANKS ((NUM_CLUSTERS < 4) ? NUM_CORES : 4)
#endif

// Number of ports per bank
#ifndef L3_NUM_PORTS
#define L3_NUM_PORTS 1
#endif

// Core Request Queue Size
#ifndef L3_CREQ_SIZE
#define L3_CREQ_SIZE 0
#endif

// Core Response Queue Size
#ifndef L3_CRSQ_SIZE
#define L3_CRSQ_SIZE 2
#endif

// Miss Handling Register Size
#ifndef L3_MSHR_SIZE
#define L3_MSHR_SIZE 16
#endif

// Memory Request Queue Size
#ifndef L3_MREQ_SIZE
#define L3_MREQ_SIZE 4
#endif

// Memory Response Queue Size
#ifndef L3_MRSQ_SIZE
#define L3_MRSQ_SIZE 0
#endif

