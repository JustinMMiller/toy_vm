#include "opcodes.h"
#include "opcodes_internal.h"

opcode_status exec_data_left(VM *vm)
{

}

opcode_status exec_data_right(VM *vm)
{

}

opcode_status exec_change_symbol(VM *vm)
{

}

opcode_status exec_branch(VM *vm)
{

}

opcode_status exec_hlt(VM *vm)
{
    vm->state = HALTED;
    return successful;
}