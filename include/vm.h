#ifndef TOY_VM_H
#define TOY_VM_H

#include "opcodes.h"

#define MAX_DATA_SIZE 0xffff
#define MAX_PROGRAM_SIZE 0xffff

typedef struct _instruction *program;

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
    unsigned short faulting_instruction;
    vm_exception_code exception;
} vm_exception;

typedef struct _VM
{
    unsigned short instruction_ptr_;
    unsigned short data_ptr_;
    program program_;
    char *tape_;
    vm_state state_;
    vm_exception exception_;
    int debug;
} VM;

int init_vm(VM *vm, void *prog, unsigned short prog_len, void *data, unsigned short data_len);
void destroy_vm(VM *vm);
// Execute a single instruction.
vm_state step(VM *vm);
// Run until hlt or exception.
vm_state run(VM *vm);
void dump_vm_state(VM *vm);

#endif