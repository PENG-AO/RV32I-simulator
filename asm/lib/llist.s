# a library for linked list

# make llist
# parameters
#   nothing
# returns
#   a0 = address
make_llist:
    # deepen stack
    addi sp, sp, -4
    sw ra, 0(sp)
    # llist struct (len/val + next)
    li a0, 2
    call malloc
    sw zero, 0(a0)
    sw zero, 4(a0)
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 4
    ret

# is empty
# parameters
#   a0 = address of llist
# returns
#   a0 = bool
llist_is_empty:
    lw a0, 0(a0) # a0 = len
    slti a0, a0, 1 # a0 = (len < 1) ? 1 : 0
    ret

# prepend to head
# parameters
#   a0 = address of llist
#   a1 = value
# returns
#   nothing
llist_prepend:
    # deepen stack
    addi sp, sp, -12
    sw ra, 0(sp)
    sw a0, 4(sp) # store head of llist
    sw a1, 8(sp) # store the saving value
    call make_llist # a0 = addr of new node
    lw t0, 8(sp) # t0 = the saving value
    sw t0, 0(a0) # set value for new node
    lw a1, 4(sp) # a1 = head of llist
    lw a2, 4(a1) # a2 = original next
    sw a2, 4(a0) # set next for new node
    sw a0, 4(a1) # set next for head
    lw t0, 0(a1) # t0 = current len
    addi t0, t0, 1
    sw t0, 0(a1) # update len
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 12
    ret

# pop the head
# parameters
#   a0 = address of llist
# returns
#   a0 = value of head
llist_pop:
    # capacity check
    lw a1, 0(a0)  # a1 = len
    beq a1, zero, llist_pop_capacity_check_end
    addi a1, a1, -1
    sw a1, 0(a0) # update len
    lw a1, 4(a0) # next
    lw a2, 4(a1) # next next
    sw a2, 4(a0) # update next for haed
    lw a0, 0(a1) # a0 = value of next
llist_pop_capacity_check_end:
    ret

# find the nth
# parameters
#   a0 = address of llist
#   a1 = n
# returns
#   a0 = the nth node
#   a1 = the prev node of nth
llist_find_nth:
    li t0, 0 # curr idx
    mv t1, a1 # end idx
    lw a1, 4(a0) # curr node
llist_find_nth_loop_start:
    bge t0, t1, llist_find_nth_loop_end
    lw a0, 4(a0) # a0 = a0.next
    lw a1, 4(a1) # a1 = a1.next
    addi t0, t0, 1
    j llist_find_nth_loop_start
llist_find_nth_loop_end:
    ret

# insert to nth
# parameters
#   a0 = address of llist
#   a1 = n
#   a2 = value
# returns
#   nothing
llist_insert:
    # deepen stack
    addi sp, sp, -16
    sw ra, 0(sp)
    # capacity check
    blt a1, zero, llist_insert_capacity_check_fail
    lw t0, 0(a0) # t0 = len
    blt t0, a1, llist_insert_capacity_check_fail
    addi t0, t0, 1 # t0 += 1
    sw t0, 0(a0) # update len
    sw a2, 4(sp) # store the saving value
    call llist_find_nth # a0 = prev, a1 = curr
    sw a0, 8(sp) # store the prev
    sw a1, 12(sp) # store the curr
    call make_llist # a0 = addr of new node
    lw t0, 4(sp) # t0 = the saving value
    sw t0, 0(a0) # set value for new node
    lw a1, 12(sp) # a1 = curr
    sw a1, 4(a0) # set next for new node
    lw a1, 8(sp) # a1 = prev
    sw a0, 4(a1) # update next for prev
    j llist_insert_capacity_check_end
llist_insert_capacity_check_fail:
    # raise capacity warning
    mv a1, a0
    li a0, 4
    ecall
llist_insert_capacity_check_end:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 16
    ret

# remove the nth item
# parameters
#   a0 = address of llist
#   a1 = n
# returns
#   nothing
llist_remove:
    # deepen stack
    addi sp, sp, -4
    sw ra, 0(sp)
    # capacity check
    blt a1, zero, llist_remove_capacity_check_fail
    lw t0, 0(a0) # t0 = len
    bge a1, t0, llist_remove_capacity_check_fail
    addi t0, t0, -1
    sw t0, 0(a0) # update len
    call llist_find_nth # a0 = prev, a1 = curr
    lw a1, 4(a1) # a1 = a1.next
    sw a1, 4(a0) # a0.next = a1.next
    j llist_remove_capacity_check_end
llist_remove_capacity_check_fail:
    # raise capacity warning
    mv a1, a0
    li a0, 4
    ecall
llist_remove_capacity_check_end:
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 4
    ret

# find the given value
# parameters
#   a0 = address of llist
#   a1 = value
# returns
#   a0 = index
llist_find:
    lw a2, 4(a0) # a2 = curr
    li a0, 0 # a0 = index of curr
llist_find_loop_start:
    beq a2, zero, llist_find_loop_end
    lw t0, 0(a2) # t0 = a2.val
    beq t0, a1, llist_find_loop_end
    addi a0, a0, 1 # update index of curr
    lw a2, 4(a2) # a2 = a2.next
    j llist_find_loop_start
llist_find_loop_end:
    # result chcek
    bne a2, zero, llist_find_result_check_end
    li a0, -1
llist_find_result_check_end:
    ret

# map
# parameters
#   a0 = address of llist
#   a1 = function for mapping
#   a2 = in place option (0 for false)
# returns
#   a0 = address of mapped llist
llist_map:
    # deepen stack
    addi sp, sp, -24
    sw ra, 0(sp)
    sw a1, 4(sp) # store map func
    sw a2, 8(sp) # store inplace option
    lw a1, 4(a0)
    sw a1, 12(sp) # store the curr node of src llist
    # prepare dst llist
    beq a2, zero, llist_map_inplace_check_end
    call make_llist # a0 = addr of dst llist
llist_map_inplace_check_end:
    sw a0, 16(sp) # store address of dst llist
    sw a0, 20(sp) # store the prev node of dst llist
llist_map_loop_start:
    lw a0, 12(sp) # a0 = curr node of src llist
    beq a0, zero, llist_map_loop_end
    lw t0, 8(sp) # t0 = inplace?
    beq t0, zero, llist_map_loop_inplace_check_end
    call make_llist # a0 = addr of new node
llist_map_loop_inplace_check_end:
    lw a1, 20(sp) # a1 = prev node of dst llist
    sw a0, 4(a1) # prev.next = a0
    lw a0, 12(sp) # a0 = curr node of src llist
    lw a1, 4(a0) # a1 = curr.next
    sw a1, 12(sp) # update curr node of src llist
    lw a0, 0(a0) # a0 = a0.val
    lw t0, 4(sp) # t0 = map func
    jalr t0 # a0 = func(curr.val)
    lw a1, 20(sp) # a1 = prev node of dst llist
    lw a1, 4(a1) # a1 = curr node of dst llist
    sw a0, 0(a1) # a1.val = a0
    sw a1, 20(sp) # update prev node of dst llist
    j llist_map_loop_start
llist_map_loop_end:
    # update len for dst llist
    li t0, 0 # len of dst llist
    lw a0, 16(sp) # a0 = addr of dst llist
    lw a0, 4(a0) # a0 = curr node of dst llist
llist_map_len_loop_start:
    beq a0, zero, llist_map_len_loop_end
    addi t0, t0, 1 # update len of dst llist
    lw a0, 4(a0) # update curr node of dst llist
    j llist_map_len_loop_start
llist_map_len_loop_end:
    lw a0, 16(sp) # a0 = addr of dst llist
    sw t0, 0(a0) # a0.len = t0
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 24
    ret

# filter
# parameters
#   a0 = address of llist
#   a1 = function for filtering
# returns
#   a0 = address of filtered llist
llist_filter:
    # deepen stack
    addi sp, sp, -24
    sw ra, 0(sp)
    sw a1, 4(sp) # store filter func
    lw a0, 4(a0)
    sw a0, 8(sp) # store the curr node of src llist
    # prepare dst llist
    call make_llist # a0 = addr of dst llist
    sw a0, 12(sp) # store address of dst llist
    sw a0, 16(sp) # store the prev node of dst llist
llist_filter_loop_start:
    lw a1, 8(sp) # a0 = curr node of src llist
    beq a1, zero, llist_filter_loop_end
    lw a0, 0(a1) # a0 = curr.val
    lw a1, 4(a1) # a1 = a1.next
    sw a1, 8(sp) # update curr node of src
    sw a0, 20(sp) # store curr.val
    lw t0, 4(sp) # t0 = filter func
    jalr t0 # a0 = func(curr.val)
    beq a0, zero, llist_filter_check_end
    call make_llist # a0 = addr of new node
    lw a1, 20(sp) # a1 = curr.val
    sw a1, 0(a0) # a0.val = a1
    lw a1, 16(sp) # a1 = prev node of dst llist
    sw a0, 4(a1) # prev.next = a0
    sw a0, 16(sp) # update prev node of dst llist
llist_filter_check_end:
    j llist_filter_loop_start
llist_filter_loop_end:
    # update len for dst llist
    li t0, 0 # len of dst llist
    lw a0, 12(sp) # a0 = addr of dst llist
    lw a0, 4(a0) # a0 = curr node of dst llist
llist_filter_len_loop_start:
    beq a0, zero, llist_filter_len_loop_end
    addi t0, t0, 1 # update len of dst llist
    lw a0, 4(a0) # update curr node of dst llist
    j llist_filter_len_loop_start
llist_filter_len_loop_end:
    lw a0, 12(sp) # a0 = addr of dst llist
    sw t0, 0(a0) # a0.len = t0
    # restore stack
    lw ra, 0(sp)
    addi sp, sp, 24
    ret
