#include "mmu.h"
#include "core.h"

const u32 mmu_read_word(const MMU* mmu, void* const core, const u32 addr) {
    u32 val;
    if (mmu->data_cache->read_word(mmu->data_cache, addr, &val)) {
        // cache miss, count stall
        ((CORE*)core)->stall_counter += 100;
        // load certain block to cache
        mmu->data_cache->load_block(mmu->data_cache, addr, mmu->data_mem);
        val = mmu->data_mem->read_word(mmu->data_mem, addr);
    }
    return val;
}

void mmu_write_word(const MMU* mmu, void* const core, const u32 addr, const u32 val) {
    if (mmu->data_cache->write_word(mmu->data_cache, addr, val)) {
        // cache miss, count stall
        ((CORE*)core)->stall_counter += 100;
        // write allocate
        mmu->data_cache->load_block(mmu->data_cache, addr, mmu->data_mem);
        mmu->data_cache->write_word(mmu->data_cache, addr, val);
    }
}

u32 mmu_sneak(MMU* mmu, u32 addr) {
    u32 val;
    if (mmu->data_cache->sneak(mmu->data_cache, addr, &val))
        val = mmu->data_mem->read_word(mmu->data_mem, addr);
    return val;
}

void init_mmu(MMU* mmu) {
    // init cache
    static CACHE data_cache;
    init_cache(&data_cache);
    mmu->data_cache = &data_cache;
    // init memory
    static MEM data_mem;
    init_mem(&data_mem);
    mmu->data_mem = &data_mem;
    // assign interfaces
    mmu->read_word = mmu_read_word;
    mmu->write_word = mmu_write_word;
    mmu->sneak = mmu_sneak;
}
