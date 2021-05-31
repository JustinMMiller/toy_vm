#include <stdio.h>
#include <signal.h>

#include "vm.h"
#include "opcodes_internal.h"

void dump_vm_state(VM *vm)
{
    printf("State : %i\n", vm->state_);
    printf("Exception : %i\n", vm->exception_.exception);
    if (vm->exception_.exception)
    {
        printf("Faulting Instruction : %i\n", vm->exception_.faulting_instruction);
    }
    printf("Current IP: %i\n", vm->instruction_ptr_);
    printf("Current data index: %i\n", vm->data_ptr_);
}

int main(int argc, char **argv)
{
    VM vm;
    instruction inst;
    printf("Beginning test of toy vm\n");
    inst.code = hlt;
    if (init_vm(&vm, &inst, 1, NULL, 0))
    {
        vm_state state = step(&vm);
        if (state != HALTED)
        {
            printf("VM did not halt.\n");
            dump_vm_state(&vm);
        }
        printf("HLT instruction behaved as expected\n");
    }
    else
    {
        printf("Failed to init vm\n");
    }
}
