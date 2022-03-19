    .globl main

    .text
main:
    # create a separate 4(row)*2(col)words-array
    li a0, 4
    call malloc
    mv s1, a0
    # init each row
    li t0, 0 # row-idx
    li t1, 4 # max-row
init_row_loop_start:
    bge t0, t1, init_row_loop_end
    li a0, 2
    call malloc
    mv a2, a0
    mv a0, s1
    mv a1, t0
    call set_nth
    addi t0, t0, 1
    j init_row_loop_start
init_row_loop_end:

    # init every item with row-idx + col-idx
    li t0, 0 # row-idx
    li t1, 4 # max-row
init_outer_loop_start:
    bge t0, t1, init_outer_loop_end
    li t2, 0 # col-idx
    li t3, 2 # max-col
init_inner_loop_start:
    bge t2, t3, init_inner_loop_end
    # set row address
    mv a0, s1
    mv a1, t0
    call get_nth
    # set column index
    mv a1, t2
    # set value (row + col)
    add a2, t0, t2
    call set_nth
    addi t2, t2, 1
    j init_inner_loop_start
init_inner_loop_end:
    addi t0, t0, 1
    j init_outer_loop_start
init_outer_loop_end:

    # print out each item
    li t0, 0 # row-idx
    li t1, 4 # row-max
print_outer_loop_start:
    bge t0, t1, print_outer_loop_end
    li t2, 0 # col-idx
    li t3, 2 # cal-max
print_inner_loop_start:
    bge t2, t3, print_inner_loop_end
    # access row
    mv a0, s1
    mv a1, t0
    call get_nth
    # access col
    mv a1, t2
    call get_nth
    # print int
    mv a1, a0
    li a0, 1
    ecall
    addi t2, t2, 1
    j print_inner_loop_start
print_inner_loop_end:
    addi t0, t0, 1
    j print_outer_loop_start
print_outer_loop_end:

    # exit
    li a0, 0
    ecall
