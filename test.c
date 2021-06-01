#include <stdio.h>
#include <signal.h>

#include "vm.h"
#include "opcodes_internal.h"

instruction test_hlt[] = {
    {hlt}
};

instruction test_dl[] = {
    {dl, 1},
    {hlt}
};

instruction test_dr[] = {
    {dr, 1},
    {hlt}
};

instruction test_cs[] = {
    {cs, 'c'},
    {hlt}
};

instruction test_bre[] = {
    {bre},
    {hlt}
};
char test_bre_data[] = {'a'};

instruction test_brne[] = {
    {brne},
    {hlt}
};
char test_brne_data[] = {'a'};

instruction test_brlt[] = {
    {brlt},
    {hlt}
};
char test_brlt_data[] = {'a'};

instruction test_brgt[] = {
    {brgt},
    {hlt}
};
char test_brgt_data[] = {'a'};

instruction test_setd[] = {
    {setd},
    {hlt}
};

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

typedef int(*check_result_func)(VM *, vm_state);

int check_hlt_step(VM *vm, vm_state state)
{
    return state == HALTED;
}

check_result_func check_hlt_run = check_hlt_step;

int check_dl_step(VM *vm, vm_state state)
{
    return state == EXCEPTION_OCCURED && vm->exception_.exception == LEFT_END_OF_TAPE_REACHED;
}

check_result_func check_dl_run = check_dl_step;

int check_dr_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->data_ptr_ == 1;
}

check_result_func check_dr_run = check_hlt_step;

int check_cs_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 'c';
}

int check_cs_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 'c';
}

int check_bre_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 1;
}

check_result_func check_bre_run = check_hlt_step;

int check_brne_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 0;
}

int check_brlt_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 1;
}

check_result_func check_brlt_run = check_hlt_step;

int check_brgt_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 0;
}

check_result_func check_setd_step = check_dr_step;
check_result_func check_setd_run = check_hlt_step;

int run_step_test(program prog,
                  unsigned short prog_len,
                  void *test_data,
                  unsigned short data_len,
                  check_result_func check_success)
{
    VM vm;
    int result = 0;
    if (init_vm(&vm, prog, prog_len, test_data, data_len))
    {
        vm_state state = step(&vm);
        result = check_success(&vm, state);
        if (!result)
        {
            dump_vm_state(&vm);
        }
        destroy_vm(&vm);
    }
    else
    {
        printf("Unable to init vm\n");
    }
    return result;
}

int run_run_test(program prog,
                  unsigned short prog_len,
                  void *test_data,
                  unsigned short data_len,
                  check_result_func check_success)
{
    VM vm;
    int result = 0;
    if (init_vm(&vm, prog, prog_len, test_data, data_len))
    {
        vm_state state = run(&vm);
        result = check_success(&vm, state);
        if (!result)
        {
            dump_vm_state(&vm);
        }
        destroy_vm(&vm);
    }
    else
    {
        printf("Unable to init vm\n");
    }
    return result;
}

int main(int argc, char **argv)
{
    VM vm;
    int res;
    printf("Beginning test of toy vm\n");
    printf("Size of instruction : %li\n", sizeof(instruction));
    printf("test_hlt\n");
    res = run_step_test(test_hlt, sizeof(test_hlt), NULL, 0, check_hlt_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_dl\n");
    res = run_step_test(test_dl, sizeof(test_dl), NULL, 0, check_dl_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_dr\n");
    res = run_step_test(test_dr, sizeof(test_dr), NULL, 0, check_dr_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_cs\n");
    res = run_step_test(test_cs, sizeof(test_cs), NULL, 0, check_cs_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_setd\n");
    test_setd[0].set_datap.pos = 1;
    res = run_step_test(test_setd, sizeof(test_setd), NULL, 0, check_setd_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_bre\n");
    res = run_step_test(test_bre, sizeof(test_bre), test_bre_data, sizeof(test_bre_data), check_bre_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_brne\n");
    res = run_step_test(test_brne, sizeof(test_brne), test_brne_data, sizeof(test_brne_data), check_brne_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_brlt\n");
    test_brlt[0].branch.pos = 0;
    test_brlt[0].branch.val = 0;
    res = run_step_test(test_brlt, sizeof(test_brlt), test_brlt_data, sizeof(test_brlt_data), check_brlt_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_brgt\n");
    test_brgt[0].branch.pos = 0;
    test_brgt[0].branch.val = 0;
    res = run_step_test(test_brgt, sizeof(test_brgt), test_brgt_data, sizeof(test_brgt_data), check_brgt_step);
    if (res)
    {
        printf("Test passed!\n");
    }

    // run tests
    // some test programs will loop forever, do not run those (test_brgt)

    printf("test_hlt_run\n");
    res = run_run_test(test_hlt, sizeof(test_hlt), NULL, 0, check_hlt_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_dl_run\n");
    res = run_run_test(test_dl, sizeof(test_dl), NULL, 0, check_dl_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_dr_run\n");
    res = run_run_test(test_dr, sizeof(test_dr), NULL, 0, check_dr_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_cs_run\n");
    res = run_run_test(test_cs, sizeof(test_cs), NULL, 0, check_cs_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_setd_run\n");
    res = run_run_test(test_setd, sizeof(test_setd), NULL, 0, check_setd_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_bre_run\n");
    res = run_run_test(test_bre, sizeof(test_bre), test_bre_data, sizeof(test_bre_data), check_bre_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_brlt_run\n");
    res = run_run_test(test_brlt, sizeof(test_brlt), test_brlt_data, sizeof(test_brlt_data), check_brlt_run);
    if (res)
    {
        printf("Test passed!\n");
    }
}
