#ifndef TOY_VM_H
#define TOY_VM_H

#include <stdbool.h>

#include "opcodes.h"

#define MAX_DATA_SIZE 0xffff
#define MAX_PROGRAM_SIZE 0xffff
#define MAX_TAPES 4

typedef struct _instruction *program;

#define TOY_VM_EXCEPTIONS \
X(NONE) \
X(UNKNOWN_OPCODE) \
X(LEFT_END_OF_TAPE_REACHED) \
X(RIGHT_END_OF_TAPE_REACHED) \
X(INVALID_TAPE_SPECIFIED) \
X(END_OF_PROGRAM_REACHED)

typedef enum
{
    #define X(except) except,
    TOY_VM_EXCEPTIONS
    #undef X
} vm_exception_code;

#define TOY_VM_STATES \
X(RUNNING) \
X(HALTED) \
X(EXCEPTION_OCCURED)

typedef enum
{
    #define X(state) state,
    TOY_VM_STATES
    #undef X
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
    char tape_idx_;
    char *tapes_[MAX_TAPES];
    vm_state state_;
    vm_exception exception_;
    bool debug;
} VM;

int init_vm(VM *vm, void *prog, unsigned short prog_len, void *data, unsigned short data_len);
void destroy_vm(VM *vm);
// Execute a single instruction.
vm_state step(VM *vm);
// Run until hlt or exception.
vm_state run(VM *vm);
void dump_vm_state(VM *vm);

#endif