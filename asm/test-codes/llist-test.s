    .globl main

    .text
main:
    # create a llist
    call make_llist
    mv s0, a0 # s0 = llist

    # prepend 1 ~ 5
    li s1, 1
    li s2, 6
prepend_loop_start:
    bge s1, s2, prepend_loop_end
    mv a0, s0
    mv a1, s1
    call llist_prepend
    addi s1, s1, 1
    j prepend_loop_start
prepend_loop_end:

    # is empty?
    mv a0, s0 # a0 = llist
    call llist_is_empty # a0 = empty?
    mv a1, a0
    li a0, 2
    ecall # print bool

    # filter
    mv a0, s0 # a0 = llist
    la a1, is_odd
    call llist_filter # a0 = filtered llist

    mv s1, a0
    li a1, 0
    li a2, 2
    call llist_insert

    mv a0, s1
    lw a1, 0(a0)
    li a2, 4
    call llist_insert

    mv a0, s1
    la a1, print_item
    li a2, 1
    call llist_map

    # find 3
    mv a0, s0 # a0 = llist
    li a1, 3
    call llist_find # a0 = index

    # remove idx of 3
    mv a1, a0 # a1 = index
    mv a0, s0 # a0 = llist
    call llist_remove

    # find 6
    mv a0, s0 # a0 = llist
    li a1, 6
    call llist_find # a0 = index

    # remove idx of 6
    mv a1, a0 # a1 = index
    mv a0, s0 # a0 = llist
    call llist_remove

    # pop until empty
pop_loop_start:
    mv a0, s0 # a0 = llist
    call llist_is_empty # a0 = empty?
    bne a0, zero, pop_loop_end
    mv a0, s0 # a0 = llist
    call llist_pop # a0 = value of head
    mv a1, a0
    li a0, 1
    ecall # print int
    j pop_loop_start
pop_loop_end:

    li a0, 0
    ecall

is_odd:
    andi a0, a0, 1
    ret

print_item:
    mv a1, a0
    li a0, 1
    ecall
    mv a0, a1
    ret
