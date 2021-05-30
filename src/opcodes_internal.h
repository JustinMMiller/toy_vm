#ifndef OPCODES_INTERNAL_H
#define OPCODES_INTERNAL_H

typedef char op;

typedef struct _gen_instr
{
    op code;
    char unused[3];
} gen_instr;

typedef struct _branch_instr
{
    op code;
    char val;
    short pos;
} branch_instr;

/**
 * @brief Update data position.
 * 
 */
typedef struct _set_dataptr
{
    op code;
    short pos;
} set_dataptr;

typedef union _instruction
{
    set_dataptr datap;
    branch_instr branch;
    gen_instr instr;
} _instruction;

#endif