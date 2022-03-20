# a library for sequential list

# make list
# parameters
#   a0 = length(in words)
# returns
#   a0 = address
make_slist:
    # deepen stack
    addi sp, sp, -8
    sw ra, 0(sp)
    sw a0, 4(sp) # store len
    # slist struct (len + max + addr of list)
    li a0, 3
    call malloc
    sw zero, 0(a0) # set len
    lw t0, 4(sp) # t0 = max len
    sw t0, 4(a0) # set max len
    # allocate list
    sw a0, 4(sp) # store slist
    lw a0, 4(a0) # a0 = max len
    call malloc # a0 = addr of list
    lw a1, 4(sp) # a1 = slist
    sw a0, 8(a1) # set addr of list
    mv a0, a1 # a0 = slist
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 8
    ret

# is empty
# parameters
#   a0 = address of slist
# returns
#   a0 = bool
slist_is_empty:
    lw a0, 0(a0) # a0 = len
    slti a0, a0, 1 # a0 = (a0 < 1) ? 1 : 0
    ret

# append to tail
# parameters
#   a0 = address of slist
#   a1 = value
# returns
#   nothing
slist_append:
    # deepen stack
    addi sp, sp, -8
    sw ra, 0(sp)
    # capacity check
    lw t0, 0(a0) # t0 = len
    lw t1, 4(a0) # t1 = max len
    bge t0, t1, slist_append_capacity_check_fail
    sw a0, 4(sp) # store slist
    lw a0, 8(a0) # a0 = addr of list
    mv a2, a1 # a2 = value
    mv a1, t0 # a1 = n (current len)
    call set_nth
    lw a0, 4(sp) # a0 = slist
    lw t0, 0(a0) # t0 = len
    addi t0, t0, 1
    sw t0, 0(a0)
    j slist_append_capacity_check_end
slist_append_capacity_check_fail:
    # raise capacity warning
    mv a1, a0
    li a0, 4
    ecall
slist_append_capacity_check_end:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 8
    ret

# pop the tail
# parameters
#   a0 = address of slist
# returns
#   a0 = value of tail
slist_pop:
    # deepen stack
    addi sp, sp, -4
    sw ra, 0(sp)
    # capacity check
    lw a1, 0(a0) # t0 = len
    beq a1, zero, slist_pop_capacity_check_end
    addi a1, a1, -1
    sw a1, 0(a0) # update len
    lw a0, 8(a0) # a0 = addr of list
    call get_nth # a0 = value of tail
slist_pop_capacity_check_end:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 4
    ret

# remove the nth item
# parameters
#   a0 = address of slist
#   a1 = n
# returns
#   nothing
slist_remove:
    # deepen stack
    addi sp, sp, -16
    sw ra, 0(sp)
    # capacity check
    blt a1, zero, slist_remove_capacity_check_fail
    lw t1, 0(a0) # t1 = len
    bge a1, t1, slist_remove_capacity_check_fail
    addi t0, t1, -1 # t0 = len - 1
    sw t0, 0(a0) # update len
    lw a0, 8(a0) # a0 = addr of list
    sw a0, 4(sp) # store addr of list
    sw a1, 8(sp) # store the current index of loop
    sw t0, 12(sp) # store the end index of loop
slist_remove_transfer_loop_start:
    lw a1, 8(sp) # a1 = current idx
    lw t1, 12(sp) # t1 = end idx
    bge a1, t1, slist_remove_transfer_loop_end
    lw a0, 4(sp) # a0 = addr of list
    addi a1, a1, 1 # a1 = current idx + 1
    call get_nth # a0 = [current idx + 1]
    mv a2, a0 # a2 = [current idx + 1]
    lw a0, 4(sp) # a0 = addr of list
    lw a1, 8(sp) # a1 = current idx
    call set_nth # [current idx] = [current idx + 1]
    lw t0, 8(sp) # t0 = current idx
    addi t0, t0, 1
    sw t0, 8(sp) # update current idx
    j slist_remove_transfer_loop_start
slist_remove_transfer_loop_end:
    j slist_remove_capacity_check_end
slist_remove_capacity_check_fail:
    # raise capacity warning
    mv a1, a0
    li a0, 4
    ecall
slist_remove_capacity_check_end:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 16
    ret

# find the given value
# parameters
#   a0 = address of slist
#   a1 = value
# returns
#   a0 = index
slist_find:
    # deepen stack
    addi sp, sp, -20
    sw ra, 0(sp)
    lw t0, 0(a0) # t0 = len
    lw a0, 8(a0) # a0 = addr of list
    sw a0, 4(sp) # store addr of list
    sw zero, 8(sp) # store the current idx of loop
    sw t0, 12(sp) # store the end idx of loop
    sw a1, 16(sp) # store the finding value
slist_find_loop_start:
    lw a1, 8(sp) # a1 = current idx
    lw t0, 12(sp) # t0 = end idx
    bge a1, t0, slist_find_loop_end
    lw a0, 4(sp) # a0 = addr of list
    call get_nth # a0 = [current idx]
    lw t0, 16(sp) # t0 = the finding value
    beq a0, t0, slist_find_loop_end
    lw t0, 8(sp) # t0 = current idx
    addi t0, t0, 1
    sw t0, 8(sp) # update current idx
    j slist_find_loop_start
slist_find_loop_end:
    # result check
    lw a0, 8(sp) # a0 = current idx
    lw t0, 12(sp) # t0 = end idx
    blt a0, t0, slist_find_result_check_pass
    li a0, -1
slist_find_result_check_pass:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 20
    ret
