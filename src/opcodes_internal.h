#ifndef OPCODES_INTERNAL_H
#define OPCODES_INTERNAL_H

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
} _instruction;

#endif
