#include <stdio.h>
#include <signal.h>

#include "vm.h"
#include "opcodes_internal.h"

typedef int(*check_result_func)(VM *, vm_state);


instruction test_hlt[] = {
    {hlt}
};

int check_hlt_step(VM *vm, vm_state state)
{
    return state == HALTED;
}

check_result_func check_hlt_run = check_hlt_step;


instruction test_dl[] = {
    {dl, {{1}}},
    {hlt}
};

int check_dl_step(VM *vm, vm_state state)
{
    // Moving the data pointer left from 0 is expected to
    // trigger a LEFT_END_OF_TAPE_REACHED exception.
    return state == EXCEPTION_OCCURED && vm->exception_.exception == LEFT_END_OF_TAPE_REACHED;
}

check_result_func check_dl_run = check_dl_step;


instruction test_dr[] = {
    {dr, {{1}}},
    {hlt}
};

int check_dr_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->data_ptr_ == 1;
}

check_result_func check_dr_run = check_hlt_step;


instruction test_madd[] = {
    {madd, {{1}}},
    {hlt}
};
char test_madd_data[] = {1, 2};

int check_madd_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 3;
}

int check_madd_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 3;
}


instruction test_msub[] = {
    {msub, {{1}}},
    {hlt}
};
char test_msub_data[] = {2, 1};

int check_msub_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 1;
}

int check_msub_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 1;
}


instruction test_iadd[] = {
    {iadd, {{1}}},
    {hlt}
};
char test_iadd_data[] = {1};

int check_iadd_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 2;
}

int check_iadd_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 2;
}


instruction test_isub[] = {
    {isub, {{1}}},
    {hlt}
};
char test_isub_data[] = {3};

int check_isub_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 2;
}

int check_isub_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 2;
}


instruction test_cs[] = {
    {cs, {{'c'}}},
    {hlt}
};

int check_cs_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->tape_[0] == 'c';
}

int check_cs_run(VM *vm, vm_state state)
{
    return state == HALTED && vm->tape_[0] == 'c';
}


instruction test_br[] = {
    {br},
    {hlt}
};

int check_br_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 0;
}


instruction test_bre[] = {
    {bre},
    {hlt}
};
char test_bre_data[] = {'a'};

int check_bre_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 1;
}

check_result_func check_bre_run = check_hlt_step;


instruction test_brne[] = {
    {brne},
    {hlt}
};
char test_brne_data[] = {'a'};

int check_brne_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 0;
}


instruction test_brlt[] = {
    {brlt},
    {hlt}
};
char test_brlt_data[] = {'a'};

int check_brlt_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 1;
}

check_result_func check_brlt_run = check_hlt_step;


instruction test_brgt[] = {
    {brgt},
    {hlt}
};
char test_brgt_data[] = {'a'};

int check_brgt_step(VM *vm, vm_state state)
{
    return state == RUNNING && vm->instruction_ptr_ == 0;
}


instruction test_setd[] = {
    {setd}, // pos set later before test case.
    {hlt}
};

check_result_func check_setd_step = check_dr_step;
check_result_func check_setd_run = check_hlt_step;

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
    printf("test_madd\n");
    res = run_step_test(test_madd, sizeof(test_madd), test_madd_data, sizeof(test_madd_data), check_madd_step);
    if (res)
    {
        printf("Test passed\n");
    }
    printf("test_msub\n");
    res = run_step_test(test_msub, sizeof(test_msub), test_msub_data, sizeof(test_msub_data), check_msub_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_iadd\n");
    res = run_step_test(test_iadd, sizeof(test_iadd), test_iadd_data, sizeof(test_iadd_data), check_iadd_step);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_isub\n");
    res = run_step_test(test_isub, sizeof(test_isub), test_isub_data, sizeof(test_isub_data), check_isub_step);
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
    printf("test_br\n");
    res = run_step_test(test_br, sizeof(test_br), NULL, 0, check_br_step);
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
    printf("test_madd_run\n");
    res = run_run_test(test_madd, sizeof(test_madd), test_madd_data, sizeof(test_madd_data), check_madd_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_iadd\n");
    res = run_run_test(test_iadd, sizeof(test_iadd), test_iadd_data, sizeof(test_iadd_data), check_iadd_run);
    if (res)
    {
        printf("Test passed!\n");
    }
    printf("test_isub\n");
    res = run_run_test(test_isub, sizeof(test_isub), test_isub_data, sizeof(test_isub_data), check_isub_run);
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
