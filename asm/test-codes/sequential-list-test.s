    .globl main

    .text
main:
    # create a 5words-slist
    li a0, 5
    call make_slist
    mv s0, a0 # s0 = slist
    ebreak

    # append 1 ~ 5
    li s1, 1
    li s2, 6
append_loop_start:
    bge s1, s2, append_loop_end
    mv a0, s0
    mv a1, s1
    call slist_append
    addi s1, s1, 1
    j append_loop_start
append_loop_end:
    ebreak

    # is empty?
    mv a0, s0 # a0 = slist
    call slist_is_empty # a0 = empty?
    mv a1, a0
    li a0, 2
    ecall # print bool
    ebreak

    # find 6
    mv a0, s0 # a0 = slist
    li a1, 6
    call slist_find
    ebreak
    mv a1, a0 # a1 = index
    mv a0, s0 # a0 = slist
    call slist_remove
    ebreak

    # remove 3
    mv a0, s0 # a0 = slist
    li a1, 3
    call slist_find # a0 = index
    ebreak
    mv a1, a0 # a1 = index
    mv a0, s0 # a0 = slist
    call slist_remove
    ebreak

    # pop until empty
pop_loop_start:
    mv a0, s0 # a0 = slist
    call slist_is_empty # a0 = empty?
    bne a0, zero, pop_loop_end
    mv a0, s0 # a0 = slist
    call slist_pop # a0 = value of tail
    mv a1, a0
    li a0, 1
    ecall # print int
    j pop_loop_start
pop_loop_end:
    ebreak

    li a0, 0
    ecall
