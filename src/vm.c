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
    vm->tape_ = calloc(sizeof(char), MAX_DATA_SIZE);
    vm->program_ = calloc(sizeof(instruction), MAX_PROGRAM_SIZE);
    memcpy(vm->program_, prog, min(MAX_PROGRAM_SIZE, prog_len));
    if (data)
    {
        memcpy(vm->tape_, data, min(data_len, MAX_DATA_SIZE));
    }
    return 1;
}

void destroy_vm(VM *vm)
{
    vm->state_ = HALTED;
    free(vm->tape_);
    vm->tape_ = NULL;
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
        case (op)dl:
        {
            status = exec_data_left(vm);
            break;
        }
        case (op)dr:
        {
            status = exec_data_right(vm);
            break;
        }
        case (op)cs:
        {
            status = exec_change_symbol(vm);
            break;
        }
        case (op)bre:
        {
            status = exec_branch_eq(vm);
            break;
        }
        case (op)brne:
        {
            status = exec_branch_ne(vm);
            break;
        }
        case (op)brlt:
        {
            status = exec_branch_lt(vm);
            break;
        }
        case (op)brgt:
        {
            status = exec_branch_gt(vm);
            break;
        }
        case (op)hlt:
        {
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