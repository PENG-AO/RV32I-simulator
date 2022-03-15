#pragma once
#include "global.h"
#include "mmu.h"
#include "branch_predictor.h"

#define DEFAULT_PC 0

static char* reg_name[32] = {
    "zero",
    "ra", "sp", "gp", "tp",
    "t0", "t1", "t2",
    "fp", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6"
};

typedef struct stdout_buf {
    char buf[16][12];
    u32 head_idx, tail_idx;
    void (*add_int)(struct stdout_buf*, s32);
    void (*add_new_line)(struct stdout_buf*);
    char* (*get_line)(struct stdout_buf*, u32);
} STDOUT_BUF;

void init_stdout_buf(STDOUT_BUF* stdout_buf);

typedef struct core {
    // attributes
    u32 pc;
    u32 regs[32];
    MMU* mmu;
    BRANCH_PREDICTOR* branch_predictor;
    // analysis
    u64 instr_counter;
    u64 stall_counter;
    u64 instr_analysis[27];
    // stdout buffer
    STDOUT_BUF* stdout_buf;
    // interfaces
    void (*step)(struct core* const);
    const u32 (*load_word)(const struct core*, const u32);
    void (*store_word)(const struct core*, const u32, const u32);
    void (*reset)(struct core*);
} CORE;

void init_core(CORE* core);
