#ifndef TOY_VM_H
#define TOY_VM_H

#include "opcodes.h"

#define MAX_DATA_SIZE 0xffff
#define MAX_PROGRAM_SIZE 0xffff

typedef enum
{
    NONE,
    UNKNOWN_OPCODE,
    LEFT_END_OF_TAPE_REACHED,
    RIGHT_END_OF_TAPE_REACHED,
    // End of program reached without a hlt instruction.
    END_OF_PROGRAM_REACHED
} vm_exception_code;

typedef enum
{
    RUNNING,
    HALTED,
    EXCEPTION_OCCURED
} vm_state;

typedef struct _vm_exception
{
    unsigned short last_successful_instruction;
    vm_exception_code exception;
} vm_exception;

typedef struct _VM
{
    unsigned short instruction_ptr;
    unsigned short data_ptr;
    void *program;
    char *tape;
    int program_size;
    vm_state state;
    vm_exception exception;
} VM;

void set_data();
char get_data();

#endif