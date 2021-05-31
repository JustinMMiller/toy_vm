#include <stdio.h>
#include <signal.h>

#include "vm.h"
#include "opcodes_internal.h"

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
            printf("State : %i, vm.state_ : %i\n", state, vm.state_);
            raise(SIGTRAP);
        }
    }
    else
    {
        printf("Failed to init vm\n");
    }
}