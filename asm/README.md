# risc-v assembler

## 1. Instruction format

<img src='https://devopedia.org/images/article/110/3808.1535301636.png' height=200>

## 2. RV32I

| name                        | assembly             | fmt | opcode  | funct3 | funct7         | description                       |
| :-------------------------- | :------------------- | :-- | :------ | :----- | :------------- | :-------------------------------- |
| **ARITH**                   |                      |     |         |        |                |                                   |
| add                         | `add rd, rs1, rs2`   | R   | 0110011 | 000    | 0000000        | rd = rs1 + rs2                    |
| sub                         | `sub rd, rs1, rs2`   | R   | 0110011 | 000    | 0100000        | rd = rs1 - rs2                    |
| shift left logical          | `sll rd, rs1, rs2`   | R   | 0110011 | 001    | 0000000        | rd = rs1 << rs2                   |
| set less than               | `slt rd, rs1, rs2`   | R   | 0110011 | 010    | 0000000        | rd = (rs1 < rs2) ? 1 : 0          |
| xor                         | `xor rd, rs1, rs2`   | R   | 0110011 | 100    | 0000000        | rd = rs1 ^ rs2                    |
| shift right logical         | `srl rd, rs1, rs2`   | R   | 0110011 | 101    | 0000000        | rd = rs1 >> rs2 (zero-ext)        |
| shift right arith           | `sra rd, rs1, rs2`   | R   | 0110011 | 101    | 0100000        | rd = rs1 >> rs2 (msb-ext)         |
| or                          | `or rd, rs1, rs2`    | R   | 0110011 | 110    | 0000000        | rd = rs1 | rs2                    |
| and                         | `and rd, rs1, rs2`   | R   | 0110011 | 111    | 0000000        | rd = rs1 & rs2                    |
| **ARITH-I**                 |                      |     |         |        |                |                                   |
| add immediate               | `addi rd, rs1, imm`  | I   | 0010011 | 000    |                | rd = rs1 + sext(imm)              |
| shift left logical imm      | `slli rd, rs1, imm`  | I   | 0010011 | 001    | imm[11:5]=0x00 | rd = rs1 << imm[4:0]              |
| set less than imm           | `slti rd, rs1, imm`  | I   | 0010011 | 010    |                | rd = (rs1 < sext(imm)) ? 1 : 0    |
| xor immediate               | `xori rd, rs1, imm`  | I   | 0010011 | 100    |                | rd = rs1 ^ sext(imm)              |
| shift right logical imm     | `srli rd, rs1, imm`  | I   | 0010011 | 101    | imm[11:5]=0x00 | rd = rs1 >> imm[4:0] (zero-ext)   |
| shift right arith imm       | `srai rd, rs1, imm`  | I   | 0010011 | 101    | imm[11:5]=0x20 | rd = rs1 >> imm[4:0] (msb-ext)    |
| or immediate                | `ori rd, rs1, imm`   | I   | 0010011 | 110    |                | rd = rs1 | sext(imm)              |
| and immediate               | `andi rd, rs1, imm`  | I   | 0010011 | 111    |                | rd = rs1 & sext(imm)              |
| **LOAD**                    |                      |     |         |        |                |                                   |
| load word                   | `lw rd, imm(rs1)`    | I   | 0000011 | 010    |                | rd = M[rs1+sext(imm)]             |
| **STORE**                   |                      |     |         |        |                |                                   |
| store word                  | `sw rs2, imm(rs1)`   | S   | 0100011 | 010    |                | M[rs1+sext(imm)] = rs2            |
| **BRANCH**                  |                      |     |         |        |                |                                   |
| branch equal                | `beq rs1, rs2, tag`  | B   | 1100011 | 000    |                | if (rs1 == rs2) PC += (&tag - pc) |
| branch not equal            | `bne rs1, rs2, tag`  | B   | 1100011 | 001    |                | if (rs1 != rs2) PC += (&tag - pc) |
| branch less than            | `blt rs1, rs2, tag`  | B   | 1100011 | 100    |                | if (rs1 < rs2) PC += (&tag - pc)  |
| branch great equal than     | `bge rs1, rs2, tag`  | B   | 1100011 | 101    |                | if (rs1 >= rs2) PC += (&tag - pc) |
| **JUMP**                    |                      |     |         |        |                |                                   |
| jump and link               | `jal rd, tag`        | J   | 1101111 |        |                | rd = PC + 4; PC += (&tag - pc)    |
| jump and link register      | `jalr rd, imm(rs1)`  | I   | 0010111 | 000    |                | rd = PC + 4; PC = rs1 + sext(imm) |
| **LUI & AUIPC**             |                      |     |         |        |                |                                   |
| load upper immediate        | `lui rd, imm`        | U   | 0110111 |        |                | rd = imm << 12                    |
| add upper immediate to PC   | `auipc rd, imm`      | U   | 0010111 |        |                | rd = PC + (imm << 12)             |
| **ENV**                     |                      |     |         |        |                |                                   |
| environment call            | `ecall`              | I   | 1110011 | 000    | imm = 0        | switch to some system calls       |
| environment break           | `ebreak`             | I   | 1110011 | 000    | imm = 1        | the break point of program        |

## 5. Pseudo instructions

| name                    | assembly         | base instruction                   |
| :---------------------- | :--------------- | :--------------------------------- |
| no operation            | `nop`            | `addi zero, zero, 0`               |
| load immediate          | `li rd, imm`     | `lui rd, %hi(imm)`                 |
|                         |                  | `addi rd, rd, %lo(imm)`            |
| load address            | `la rd, tag`     | `auipc rd, %hi(&tag)`              |
|                         |                  | `addi rd, rd, %lo(&tag)`           |
| one's complement        | `not rd, rs1`    | `xori rd, rs1, -1`                 |
| copy register           | `mv rd, rs1`     | `addi rd, rs1, 0`                  |
| jump                    | `j tag`          | `jal zero, tag`                    |
| jump and link           | `jal tag`        | `jal ra, tag`                      |
| jump and link register  | `jalr rs`        | `jalr ra, 0(rs)`                   |
| call faraway subroutine | `call tag`       | `auipc t1, %hi(&tag - pc)`         |
|                         |                  | `jalr ra, %lo(&tag - pc)(t1)`      |
| tail faraway subroutine | `tail tag`       | `auipc t1, %hi(&tag -pc)`          |
|                         |                  | `jalr zero, %lo(&tag - pc)(t1)`    |

## 6. Supported system call

| name           | parameters        |
| :------------- | :---------------- |
| exit           | a0 = 0            |
| print int      | a0 = 1, a1 = int  |
| print new line | a0 = 2            |
