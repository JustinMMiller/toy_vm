#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "opcodes_internal.h"
#include "vm.h"

void print_tape(VM *vm)
{
    for (int i = 0; i < MAX_DATA_SIZE; i++)
    {
        printf("%i : %x %c\n", i, vm->tape_[i], vm->tape_[i]);
    }
}

int main(int argc, char **argv)
{
    FILE *fp;
    VM vm;
    size_t data_read;
    toy_file toy_bin = {0};
    program toy = NULL;
    void *initial_tape = NULL;
    if (argc != 2)
    {
        printf("loader takes exactly 1 argument\n");
        return __LINE__;
    }
    fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("Unable to open file %s for reading\n", argv[1]);
        return __LINE__;
    }

    data_read = fread(&toy_bin, sizeof(toy_file), 1, fp);

    printf("Read %lx bytes : program_len : %i data_size : %i\n", data_read, toy_bin.program_len, toy_bin.data_len);

    toy = calloc(toy_bin.program_len, sizeof(instruction));

    data_read = fread(toy, sizeof(instruction), toy_bin.program_len, fp);

    if (toy_bin.data_len)
    {
        initial_tape = calloc(toy_bin.data_len, sizeof(char));
        data_read = fread(initial_tape, sizeof(char), toy_bin.data_len, fp);
    }

    init_vm(&vm, toy, toy_bin.program_len, initial_tape, toy_bin.data_len);

    vm_state res = run(&vm);

    dump_vm_state(&vm);

    //print_tape(&vm);
    printf("%s\n", vm.tape_);

    if (fp)
    {
        fclose(fp);
    }
    return res != HALTED;
}