#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"
#include "opcodes.h"
#include "opcodes_internal.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

int init_vm(VM *vm, void *prog, unsigned short prog_len, void *data, unsigned short data_len)
{
    if (!prog)
    {
        return 0;
    }
    vm->data_ptr_ = 0;
    vm->exception_.exception = NONE;
    vm->exception_.faulting_instruction = 0;
    vm->instruction_ptr_ = 0;
    vm->state_ = RUNNING;
    for (int i = 0; i < MAX_TAPES; i++)
    {
        vm->tapes_[i] = NULL;
    }
    vm->tape_idx_ = 0;
    vm->tapes_[vm->tape_idx_] = calloc(MAX_DATA_SIZE+1, sizeof(char));
    vm->program_ = calloc(sizeof(instruction), MAX_PROGRAM_SIZE+1);
    memcpy(vm->program_, prog, min(MAX_PROGRAM_SIZE+1, prog_len) * sizeof(instruction));
    if (data)
    {
        memcpy(vm->tapes_[vm->tape_idx_], data, min(data_len, MAX_DATA_SIZE+1));
    }
    vm->debug = false;
    return 1;
}

void dump_vm_state(VM *vm)
{
    printf("State : %i\n", vm->state_);
    printf("Exception : %i\n", vm->exception_.exception);
    if (vm->exception_.exception != NONE)
    {
        printf("Faulting Instruction : %i\n", vm->exception_.faulting_instruction);
        printf("opcode : %x\n", vm->program_[vm->exception_.faulting_instruction].code);
    }
    printf("Current IP: %i\n", vm->instruction_ptr_);
    printf("Current data index: %i\n", vm->data_ptr_);
}

void destroy_vm(VM *vm)
{
    vm->state_ = HALTED;
    for (int i = 0; i < MAX_TAPES; i++)
    {
        if (vm->tapes_[i])
        {
            free(vm->tapes_[i]);
        }
    }
    free(vm->program_);
    vm->program_ = NULL;
}

vm_state step(VM *vm)
{
    vm_state state = RUNNING;
    unsigned short initial_ip = vm->instruction_ptr_;
    opcode_status status = successful;
    vm_exception_code except = NONE;
    if (vm->state_ == HALTED)
    {
        return vm->state_;
    }
    switch (vm->program_[vm->instruction_ptr_].code)
    {
        case (op)iadd:
        {
            if (vm->debug)printf("iadd %i\n", vm->program_[vm->instruction_ptr_].data_adj.imm);
            status = exec_data_add(vm);
            break;
        }
        case (op)isub:
        {
            if (vm->debug)printf("isub %i\n", vm->program_[vm->instruction_ptr_].data_adj.imm);
            status = exec_data_sub(vm);
            break;
        }
        case (op)dl:
        {
            if (vm->debug)printf("dl %i\n", vm->program_[vm->instruction_ptr_].shift_data_ptr.shift);
            status = exec_data_left(vm);
            break;
        }
        case (op)dr:
        {
            if (vm->debug)printf("dr %i\n", vm->program_[vm->instruction_ptr_].shift_data_ptr.shift);
            status = exec_data_right(vm);
            break;
        }
        case (op)dt:
        {
            if (vm->debug)printf("dt %i\n", vm->program_[vm->instruction_ptr_].shift_data_ptr.shift);
            status = exec_set_data_tape(vm);
            break;
        }
        case (op)madd:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("madd %i\n", i.mem_arith.loc);
            status = exec_mem_add(vm);
            break;
        }
        case (op)msub:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("msub %i\n", i.mem_arith.loc);
            status = exec_mem_sub(vm);
            break;
        }
        case (op)cs:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("cs %i\n", i.change_sym.sym);
            status = exec_change_symbol(vm);
            break;
        }
        case (op)br:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("br %i\n", i.branch.pos);
            status = exec_branch(vm);
            break;
        }
        case (op)bre:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("bre %i %i\n", i.branch.val, i.branch.pos);
            status = exec_branch_eq(vm);
            break;
        }
        case (op)brne:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("brne %i %i\n", i.branch.val, i.branch.pos);
            status = exec_branch_ne(vm);
            break;
        }
        case (op)brlt:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("brlt %i %i\n", i.branch.val, i.branch.pos);
            status = exec_branch_lt(vm);
            break;
        }
        case (op)brgt:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("brgt %i %i\n", i.branch.val, i.branch.pos);
            status = exec_branch_gt(vm);
            break;
        }
        case (op)setd:
        {
            instruction i = vm->program_[vm->instruction_ptr_];
            if (vm->debug)printf("setd %i\n", i.set_datap.pos);
            status = exec_set_dataptr(vm);
            break;
        }
        case (op)hlt:
        {
            if (vm->debug)printf("hlt\n");
            status = exec_hlt(vm);
            state = HALTED;
            break;
        }
        default:
        {
            except = UNKNOWN_OPCODE;
            state = EXCEPTION_OCCURED;
            vm->exception_.exception = except;
            vm->exception_.faulting_instruction = initial_ip;
            break;
        }
    }
    if (status != successful)
    {
        switch (status)
        {
            case exc_left_edge:
            {
                except = LEFT_END_OF_TAPE_REACHED;
                break;
            }
            case exc_right_edge:
            {
                except = RIGHT_END_OF_TAPE_REACHED;
                break;
            }
            case exc_last_instr:
            {
                except = END_OF_PROGRAM_REACHED;
                break;
            }
            case exc_invalid_tape:
            {
                except = INVALID_TAPE_SPECIFIED;
                break;
            }
            default:
            {
                return vm->state_;
            }
        }
        state = EXCEPTION_OCCURED;
        vm->exception_.exception = except;
        vm->exception_.faulting_instruction = initial_ip;
    }
    vm->state_ = state;
    return vm->state_;
}

vm_state run(VM *vm)
{
    vm_state state = RUNNING;
    while(state == RUNNING)
    {
        state = step(vm);
    }
    return state;
}