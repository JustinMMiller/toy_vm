#include "vm.h"
#include "opcodes.h"
#include "opcodes_internal.h"

opcode_status advance_inst_ptr(VM *vm)
{
    if (vm->instruction_ptr_ == MAX_PROGRAM_SIZE)
    {
        return exc_last_instr;
    }
    vm->instruction_ptr_ += 1;
    return successful;
}

opcode_status exec_data_add(VM *vm)
{
    instruction da = vm->program_[vm->instruction_ptr_];
    vm->tape_[vm->data_ptr_] += da.data_adj.adj;
    return advance_inst_ptr(vm);
}

opcode_status exec_data_sub(VM *vm)
{
    instruction da = vm->program_[vm->instruction_ptr_];
    vm->tape_[vm->data_ptr_] -= da.data_adj.adj;
    return advance_inst_ptr(vm);
}

opcode_status exec_mem_add(VM *vm)
{
    instruction ma = vm->program_[vm->instruction_ptr_];
    vm->tape_[vm->data_ptr_] += vm->tape_[ma.mem_arith.loc];
    return advance_inst_ptr(vm);
}

opcode_status exec_mem_sub(VM *vm)
{
    instruction ma = vm->program_[vm->instruction_ptr_];
    vm->tape_[vm->data_ptr_] -= vm->tape_[ma.mem_arith.loc];
    return advance_inst_ptr(vm);
}

opcode_status exec_data_left(VM *vm)
{
    instruction dps = vm->program_[vm->instruction_ptr_];
    if (vm->data_ptr_ > dps.shift_data_ptr.shift)
    {
        vm->data_ptr_ -= dps.shift_data_ptr.shift;
        return advance_inst_ptr(vm);
    }
    else
    {
        // Left edge of tape hit.
        return exc_left_edge;
    }
}

opcode_status exec_data_right(VM *vm)
{
    instruction dps = vm->program_[vm->instruction_ptr_];
    unsigned int spec_adj = vm->data_ptr_;
    // Add the shift as a separate operation to ensure
    // this is treated as integer addition instead of shorts.
    spec_adj += dps.shift_data_ptr.shift;
    if (spec_adj < (unsigned int)MAX_DATA_SIZE)
    {
        vm->data_ptr_ += dps.shift_data_ptr.shift;
        return advance_inst_ptr(vm);
    }
    else
    {
        // Right edge of tape hit.
        return exc_right_edge;
    }
}

opcode_status exec_change_symbol(VM *vm)
{
    instruction cs = vm->program_[vm->instruction_ptr_];
    vm->tape_[vm->data_ptr_] = cs.change_sym.sym;
    return advance_inst_ptr(vm);
}

opcode_status exec_branch(VM *vm)
{
    instruction br = vm->program_[vm->instruction_ptr_];
    vm->instruction_ptr_ = br.branch.pos;
    return successful;
}

opcode_status exec_branch_eq(VM *vm)
{
    instruction br = vm->program_[vm->instruction_ptr_];
    char data = vm->tape_[vm->data_ptr_];
    if (br.branch.val == data)
    {
        vm->instruction_ptr_ = br.branch.pos;
        return successful;
    }
    else
    {
        return advance_inst_ptr(vm);
    }
}

opcode_status exec_branch_ne(VM *vm)
{
    instruction br = vm->program_[vm->instruction_ptr_];
    char data = vm->tape_[vm->data_ptr_];
    if (data != br.branch.val)
    {
        vm->instruction_ptr_ = br.branch.pos;
        return successful;
    }
    else
    {
        return advance_inst_ptr(vm);
    }
}

opcode_status exec_branch_gt(VM *vm)
{
    instruction br = vm->program_[vm->instruction_ptr_];
    char data = vm->tape_[vm->data_ptr_];
    if (data > br.branch.val)
    {
        vm->instruction_ptr_ = br.branch.pos;
        return successful;
    }
    else
    {
        return advance_inst_ptr(vm);
    }
}

opcode_status exec_branch_lt(VM *vm)
{
    instruction br = vm->program_[vm->instruction_ptr_];
    char data = vm->tape_[vm->data_ptr_];
    if (data < br.branch.val)
    {
        vm->instruction_ptr_ = br.branch.pos;
        return successful;
    }
    else
    {
        return advance_inst_ptr(vm);
    }
}

opcode_status exec_set_dataptr(VM *vm)
{
    instruction sd = vm->program_[vm->instruction_ptr_];
    vm->data_ptr_ = sd.set_datap.pos;
    return advance_inst_ptr(vm);
}

opcode_status exec_hlt(VM *vm)
{
    vm->state_ = HALTED;
    return successful;
}