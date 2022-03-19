    .globl main

    .text
main:
    # create a 5words-array
    li a0, 5
    call malloc
    mv s1, a0 # s1 = u32[5]

    # init every item with its index
    li t0, 0
    li t1, 5
init_loop_start:
    bge t0, t1, init_loop_end
    mv a0, s1
    mv a1, t0
    mv a2, t0
    call set_nth # s0[t0] = t0
    addi t0, t0, 1
    j init_loop_start
init_loop_end:

    # print out each item
    li t0, 0
    li t1, 5
print_loop_start:
    bge t0, t1, print_loop_end
    # access item
    mv a0, s1
    mv a1, t0
    call get_nth
    # print int
    mv a1, a0
    li a0, 1
    ecall
    addi t0, t0, 1
    j print_loop_start
print_loop_end:

    # exit
    li a0, 0
    ecall
