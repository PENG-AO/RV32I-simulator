    .globl main

    .text
main:
    # create a 5words-array
    li a0, 5
    call malloc
    mv s0, a0 # s0 = u32[5]

    # init every item with its index
    li s1, 0
    li s2, 5
init_loop_start:
    bge s1, s2, init_loop_end
    mv a0, s0
    mv a1, s1
    mv a2, s1
    call set_nth # s0[s1] = s1
    addi s1, s1, 1
    j init_loop_start
init_loop_end:

    # print out each item
    li s1, 0
    li s2, 5
print_loop_start:
    bge s1, s2, print_loop_end
    # access item
    mv a0, s0
    mv a1, s1
    call get_nth
    # print int
    mv a1, a0
    li a0, 1
    ecall
    addi s1, s1, 1
    j print_loop_start
print_loop_end:

    # exit
    li a0, 0
    ecall
