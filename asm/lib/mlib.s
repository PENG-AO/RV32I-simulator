# a library for dynamical memory opertation

# malloc
# parameters
#   a0 = length(in words)
# returns
#   a0 = address
malloc:
    sw a1, -4(sp)
    slli a1, a0, 2
    mv a0, gp
    add gp, gp, a1
    lw a1, -4(sp)
    ret

# set the nth item
# parameters
#   a0 = address of head
#   a1 = n
#   a2 = value
# returns
#   nothing
set_nth:
    slli a1, a1, 2
    add a0, a0, a1
    sw a2, 0(a0)
    ret

# get the nth item
# parameters
#   a0 = address of head
#   a1 = n
# returns
#   a0 = a0[a1]
get_nth:
    slli a1, a1, 2
    add a0, a0, a1
    lw a0, 0(a0)
    ret
