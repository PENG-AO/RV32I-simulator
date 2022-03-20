#pragma once
#include "global.h"
#include "mem.h"
#include "cache.h"

typedef struct mmu {
    // attributes
    CACHE* data_cache;
    MEM* data_mem;
    // interfaces
    const u32 (*read_word)(const struct mmu*, void* const, const u32);
    void (*write_word)(const struct mmu*, void* const, const u32, const u32);
    u32 (*sneak)(struct mmu*, u32);
} MMU;

void init_mmu(MMU* mmu);
