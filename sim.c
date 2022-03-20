#include "sim.h"

void sim_load(SIM* sim, char* bin_name) {
    FILE* file = fopen(bin_name, "rb");
    if (file == NULL) {
        printf("invalid binary file: %s.\n", bin_name);
        exit(-1);
    } else {
        fseek(file, 0, SEEK_END);
        u64 file_size = (u64)ftell(file) >> 2;
        fseek(file, 0, SEEK_SET);
        u32 word = 0;
        u32 addr = DEFAULT_PC;
        for (s32 i = 0; i < file_size; ++i) {
            fread(&word, 1, 4, file);
            // write directly to mem
            sim->core->mmu->data_mem->write_word(sim->core->mmu->data_mem, addr, word);
            addr += 4;
        }
        // set gp for simulator
        sim->core->regs[3] = addr;
        fclose(file);
    }
}

void sim_run(SIM* const sim) {
    sim->gui->activate(sim->gui);
    // main loop of simulator
    for (;;) {
        switch (BROADCAST.decoder.type) {
        case STAT_STEP:
            if (BROADCAST.decoder.info > 0) {
                // step forward
                sim->core->step(sim->core);
                --BROADCAST.decoder.info;
            } else {
                BROADCAST(STAT_HALT);
            }
            break;
        case STAT_HALT:
            BROADCAST(sim->gui->update(sim->gui, sim->core));
            break;
        case STAT_EXIT:
        case STAT_MEM_EXCEPTION:
        case STAT_INSTR_EXCEPTION:
            BROADCAST(sim->gui->update(sim->gui, sim->core));
            if (BROADCAST.decoder.type != STAT_QUIT)
                BROADCAST(STAT_EXIT);
            break;
        case STAT_QUIT:
            sim->gui->deinit(sim->gui);
            return;
        default:
            break;
        }
    }
}

void init_sim(SIM* sim) {
    // init mmu
    static MMU mmu;
    init_mmu(&mmu);
    // init branch predictor
    static BRANCH_PREDICTOR branch_predictor;
    init_branch_predictor(&branch_predictor);
    // init stdout buf
    static STDOUT_BUF stdout_buf;
    init_stdout_buf(&stdout_buf);
    // init core
    static CORE core;
    init_core(&core);
    sim->core = &core;
    sim->core->mmu = &mmu;
    sim->core->branch_predictor = &branch_predictor;
    sim->core->stdout_buf = &stdout_buf;
    // init GUI
    static GUI gui;
    init_gui(&gui);
    sim->gui = &gui;
    // assign interfaces
    sim->load = sim_load;
    sim->run = sim_run;
    // broadcast state
    BROADCAST(STAT_HALT);
}
