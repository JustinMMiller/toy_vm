#ifndef OPCODES_INTERNAL_H
#define OPCODES_INTERNAL_H

#include "vm.h"
#include "vm_internal.h"

typedef char op;

/**
 * @brief generic instruction layout.
 */
typedef struct _gen_instr
{
    op code;
} gen_instr;

/**
 * @brief branch instruction layout.
 */
typedef struct _branch_instr
{
    op code;
    char val;
    short pos;
} branch_instr;

/**
 * @brief Update data position.
 */
typedef struct _set_dataptr
{
    op code;
    short pos;
} set_dataptr;

typedef union _instruction
{
    gen_instr instr;
    set_dataptr datap;
    branch_instr branch;
} instruction;

// opcode handlers

opcode_status exec_data_left(VM *vm);
opcode_status exec_data_right(VM *vm);
opcode_status exec_change_symbol(VM *vm);
opcode_status exec_branch(VM *vm);
opcode_status exec_hlt(VM *vm);

#endif