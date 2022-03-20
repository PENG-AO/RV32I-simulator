#include "mem.h"

typedef union mem_addr_helper {
    u32 raw;

    struct addr_decoder {
        u32        : 2;
        u32 offset : INDEX_3_LEN;
        u32 index2 : INDEX_2_LEN;
        u32 index1 : INDEX_1_LEN;
        u32        : 6;
    } __attribute__((packed)) d;
} MEM_ADDR_HELPER;

void mem_assure_page(MEM* const mem, const u32 addr) {
    register const MEM_ADDR_HELPER helper = { .raw = addr };
    if (!mem->data[helper.d.index1]) {
        mem->data[helper.d.index1] = (u32**)malloc(PAGE_2_SIZE * sizeof(u32*));
        memset(mem->data[helper.d.index1], 0, PAGE_2_SIZE * sizeof(u32*));
    }
    if (!mem->data[helper.d.index1][helper.d.index2]) {
        mem->data[helper.d.index1][helper.d.index2] = (u32*)malloc(PAGE_3_SIZE * sizeof(u32));
        memset(mem->data[helper.d.index1][helper.d.index2], 0, PAGE_3_SIZE * sizeof(u32));
    }
}

const u32 mem_read_word(const MEM* mem, const u32 addr) {
    register const MEM_ADDR_HELPER helper = { .raw = addr };
    if (addr >= MAX_ADDR) {
        BROADCAST(STAT_MEM_EXCEPTION | ((u64)addr << STAT_SHIFT_AMOUNT));
        return 0;
    } else if (mem->data[helper.d.index1] && mem->data[helper.d.index1][helper.d.index2]) {
        return mem->data[helper.d.index1][helper.d.index2][helper.d.offset];
    } else {
        return 0;
    }
}

void mem_write_word(MEM* const mem, const u32 addr, const u32 val) {
    if (!(addr < MAX_ADDR)) {
        BROADCAST(STAT_MEM_EXCEPTION | ((u64)addr << STAT_SHIFT_AMOUNT));
    } else {
        mem_assure_page(mem, addr);
        register const MEM_ADDR_HELPER helper = { .raw = addr };
        mem->data[helper.d.index1][helper.d.index2][helper.d.offset] = val;
    }
}

void init_mem(MEM* mem) {
    // check index length
    if (INDEX_1_LEN + INDEX_2_LEN + INDEX_3_LEN != 24) {
        printf("mem: invalid setting of index length.\n");
        exit(-1);
    }
    // set up parameters
    memset(mem->data, 0, PAGE_1_SIZE * sizeof(u32**));
    // assign interfaces
    mem->read_word = mem_read_word;
    mem->write_word = mem_write_word;
}
