#ifndef OPCODES_INTERNAL_H
#define OPCODES_INTERNAL_H

#include "vm.h"
#include "vm_internal.h"

typedef int op;

/**
 * @brief Branch instruction layout.
 */
typedef struct _branch_instr
{
    op code;
    char val;
    short pos;
} branch_instr;

/**
 * @brief Data pointer instruction layout.
 */
typedef struct _set_dataptr
{
    op code;
    short pos;
} set_dataptr;

/**
 * @brief Change symbol instruction layout.
 */
typedef struct _change_symbol
{
    char sym;
} change_symbol;

typedef struct _instruction
{
    op code;
    union
    {
        set_dataptr datap;
        branch_instr branch;
        change_symbol change_sym;
    };
} instruction;

// opcode handlers

opcode_status exec_data_left(VM *vm);
opcode_status exec_data_right(VM *vm);
opcode_status exec_change_symbol(VM *vm);
opcode_status exec_branch_eq(VM *vm);
opcode_status exec_branch_ne(VM *vm);
opcode_status exec_branch_lt(VM *vm);
opcode_status exec_branch_gt(VM *vm);
opcode_status exec_hlt(VM *vm);

#endif