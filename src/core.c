#include "core.h"
#include "instr.h"

void stdout_buf_add_int(STDOUT_BUF* stdout_buf, s32 val) {
    sprintf(stdout_buf->buf[stdout_buf->tail_idx++ % 16], "%d", val);
    stdout_buf->head_idx = max((s32)stdout_buf->tail_idx - 16, 0);
}

void stdout_buf_add_new_line(STDOUT_BUF* stdout_buf) {
    sprintf(stdout_buf->buf[stdout_buf->tail_idx++ % 16], "\n");
    stdout_buf->head_idx = max((s32)stdout_buf->tail_idx - 16, 0);
}

char* stdout_buf_get_line(STDOUT_BUF* stdout_buf, u32 lineno) {
    return stdout_buf->buf[(stdout_buf->head_idx + lineno) % 16];
}

void init_stdout_buf(STDOUT_BUF* stdout_buf) {
    // init basic info
    memset(stdout_buf->buf, 0, 16 * 12 * sizeof(char));
    stdout_buf->head_idx = 0;
    stdout_buf->tail_idx = 0;
    // assign interfaces
    stdout_buf->add_int = stdout_buf_add_int;
    stdout_buf->add_new_line = stdout_buf_add_new_line;
    stdout_buf->get_line = stdout_buf_get_line;
}

#define get_lw_stall(rd, op) ((!rd) || ((rd ^ ((op >> 15) & 0x1F)) && (rd ^ ((op >> 20) & 0x1F))) ? 0 : 1)

void core_step(CORE* const core) {
    // fetch
    register const INSTR instr = { .raw = core->load_word(core, core->pc) };
    // decode
    register const u32 opcode = instr.r.opcode;
    register const u32 rd = instr.r.rd;
    register const u32 rs1 = instr.r.rs1;
    register const u32 rs2 = instr.r.rs2;
    register const u32 funct3 = instr.r.funct3;
    register const u32 funct7 = instr.r.funct7;
    // execute
    register u32 imm, tmp;

    switch (opcode) {
    // lui
    case 0b0110111:
        imm = instr.u.imm31_12;
        core->regs[rd] = imm << 12;
        core->pc += 4;
        ++core->instr_analysis[LUI];
        break;
    // auipc
    case 0b0010111:
        imm = instr.u.imm31_12;
        core->regs[rd] = core->pc + (imm << 12);
        core->pc += 4;
        ++core->instr_analysis[AUIPC];
        break;
    // jal
    case 0b1101111:
        imm = instr.j.imm20 << 20 | instr.j.imm19_12 << 12 | instr.j.imm11 << 11 | instr.j.imm10_1 << 1;
        core->regs[rd] = core->pc + 4;
        core->pc += sext(imm, 20);
        ++core->instr_analysis[JAL];
        break;
    // jalr
    case 0b1100111:
        imm = instr.i.imm;
        register const u32 t = core->pc + 4;
        core->pc = core->regs[rs1] + sext(imm, 11);
        core->regs[rd] = t;
        // count stall
        core->stall_counter += 2;
        ++core->instr_analysis[JALR];
        break;
    // branch
    case 0b1100011:
        imm = instr.b.imm12 << 12 | instr.b.imm11 << 11 | instr.b.imm10_5 << 5 | instr.b.imm4_1 << 1;
        switch (funct3) {
        // beq
        case 0b000: tmp = (core->regs[rs1] == core->regs[rs2]) ? 1 : 0; break;
        // bge
        case 0b101: tmp = ((s32)core->regs[rs1] >= (s32)core->regs[rs2]) ? 1 : 0; break;
        // bne
        case 0b001: tmp = (core->regs[rs1] != core->regs[rs2]) ? 1 : 0; break;
        // blt
        case 0b100: tmp = ((s32)core->regs[rs1] < (s32)core->regs[rs2]) ? 1 : 0; break;
        // unexpected
        default: BROADCAST(STAT_INSTR_EXCEPTION | ((u64)instr.raw << STAT_SHIFT_AMOUNT)); break;
        }
        // predict branch and count stall
        core->stall_counter += core->branch_predictor->get_branch_stall(core->branch_predictor, core->pc, tmp);
        // increment pc
        core->pc += tmp ? sext(imm, 12) : 4;
        ++core->instr_analysis[BRANCH];
        break;
    // load
    case 0b0000011:
        imm = instr.i.imm;
        core->regs[rd] = core->load_word(core, core->regs[rs1] + sext(imm, 11));
        core->pc += 4;
        // fetch next instr and count stall
        tmp = core->mmu->sneak(core->mmu, core->pc);
        core->stall_counter += get_lw_stall(rd, tmp);
        ++core->instr_analysis[LOAD];
        break;
    // store
    case 0b0100011:
        imm = instr.s.imm11_5 << 5 | instr.s.imm4_0;
        core->store_word(core, core->regs[rs1] + sext(imm, 11), core->regs[rs2]);
        core->pc += 4;
        ++core->instr_analysis[STORE];
        break;
    // arith I
    case 0b0010011:
        imm = instr.i.imm;
        switch (funct3) {
        // addi
        case 0b000: core->regs[rd] = core->regs[rs1] + sext(imm, 11); break;
        // slli (legal when shamt[5] = 0, but not implemented)
        case 0b001: core->regs[rd] = core->regs[rs1] << sext(imm, 11); break;
        #if !defined(LITE)
        // slti
        case 0b010: core->regs[rd] = ((s32)core->regs[rs1] < (s32)sext(imm, 11)) ? 1 : 0; break;
        // xori
        case 0b100: core->regs[rd] = core->regs[rs1] ^ sext(imm, 11); break;
        // srli + srai (same with slli)
        case 0b101:
            if (imm >> 5)
                core->regs[rd] = (u32)(((s32)core->regs[rs1]) >> sext(imm, 11));
            else
                core->regs[rd] = core->regs[rs1] >> sext(imm, 11);
            break;
        // ori
        case 0b110: core->regs[rd] = core->regs[rs1] | sext(imm, 11); break;
        // andi
        case 0b111: core->regs[rd] = core->regs[rs1] & sext(imm, 11); break;
        #endif
        // unexpected
        default: BROADCAST(STAT_INSTR_EXCEPTION | ((u64)instr.raw << STAT_SHIFT_AMOUNT)); break;
        }
        core->pc += 4;
        ++core->instr_analysis[ARITH_I];
        break;
    // arith
    case 0b0110011:
        switch (funct3) {
        // add + sub
        case 0b000:
            if (funct7)
                core->regs[rd] = core->regs[rs1] - core->regs[rs2];
            else
                core->regs[rd] = core->regs[rs1] + core->regs[rs2];
            break;
        // sll
        case 0b001: core->regs[rd] = core->regs[rs1] << core->regs[rs2]; break;
        // or
        case 0b110: core->regs[rd] = core->regs[rs1] | core->regs[rs2]; break;
        // srl + sra
        case 0b101:
            if (funct7)
                core->regs[rd] = (u32)(((s32)core->regs[rs1]) >> core->regs[rs2]);
            else
                core->regs[rd] = core->regs[rs1] >> core->regs[rs2];
            break;
        // slt
        case 0b010: core->regs[rd] = ((s32)core->regs[rs1] < (s32)core->regs[rs2]) ? 1 : 0; break;
        // xor
        case 0b100: core->regs[rd] = core->regs[rs1] ^ core->regs[rs2]; break;
        // and
        case 0b111: core->regs[rd] = core->regs[rs1] & core->regs[rs2]; break;
        // unexpected
        default: BROADCAST(STAT_INSTR_EXCEPTION | ((u64)instr.raw << STAT_SHIFT_AMOUNT)); break;
        }
        core->pc += 4;
        ++core->instr_analysis[ARITH];
        break;    
    // env + csr
    case 0b1110011:
        imm = instr.i.imm;
        switch (imm) {
        // ecall
        case 0:
            switch (core->regs[10]) {
            case 0: // exit
                BROADCAST(STAT_EXIT);
                return;
            case 1: // print int
                core->stdout_buf->add_int(core->stdout_buf, core->regs[11]);
                break;
            case 2: // print new line
                core->stdout_buf->add_new_line(core->stdout_buf);
                break;
            }
            break;
        // ebreak
        case 1: BROADCAST(STAT_HALT); break;
        // unexpected
        default: BROADCAST(STAT_INSTR_EXCEPTION | ((u64)instr.raw << STAT_SHIFT_AMOUNT)); break;
        }
        core->pc += 4;
        break;
    // unexpected
    default: BROADCAST(STAT_INSTR_EXCEPTION | ((u64)instr.raw << STAT_SHIFT_AMOUNT)); break;
    }
    // after work
    core->regs[0] = 0;
    ++core->instr_counter;
}

const u32 core_load_word(const CORE* core, const u32 addr) {
    return core->mmu->read_word(core->mmu, (void* const)core, addr);
}

void core_store_word(const CORE* core, const u32 addr, const u32 val) {
    core->mmu->write_word(core->mmu, (void* const)core, addr, val);
}

void init_core(CORE* core) {
    // init basic info
    core->pc = DEFAULT_PC;
    core->regs[2] = MAX_ADDR;
    core->instr_counter = 0;
    core->stall_counter = 0;
    memset(core->instr_analysis, 0, INSTR_TYPES * sizeof(u64));
    // assign interfaces
    core->load_word = core_load_word;
    core->store_word = core_store_word;
    core->step = core_step;
}
