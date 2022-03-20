#pragma once
#include "global.h"

#define MAX_ADDR 0x00100000

// address
// 31 ~ 26 (6): disused
// 25 ~ 18 (8): 1st index
// 17 ~ 10 (8): 2nd index
// 9  ~ 2  (8): 3rd index
// 1  ~ 0  (2): disused

#define INDEX_1_LEN 2
#define INDEX_2_LEN 6
#define INDEX_3_LEN 16

#define PAGE_1_SIZE (1 << INDEX_1_LEN)
#define PAGE_2_SIZE (1 << INDEX_2_LEN)
#define PAGE_3_SIZE (1 << INDEX_3_LEN)

typedef struct mem {
    u32** data[PAGE_1_SIZE];
    // interfaces
    const u32 (*read_word)(const struct mem*, const u32);
    void (*write_word)(struct mem* const, const u32, const u32);
} MEM;

void init_mem(MEM* mem);
