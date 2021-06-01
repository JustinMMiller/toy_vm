#ifndef OPCODES_INTERNAL_H
#define OPCODES_INTERNAL_H

#include "vm.h"
#include "vm_internal.h"

typedef char op;

/**
 * @brief Branch instruction layout.
 */
typedef struct _branch_instr
{
    char val;
    short pos;
} __attribute__((packed)) branch_instr;

/**
 * @brief Data pointer instruction layout.
 */
typedef struct _set_dataptr
{
    short pos;
} __attribute__((packed)) set_dataptr;

/**
 * @brief Change symbol instruction layout.
 */
typedef struct _change_symbol
{
    char sym;
} __attribute__((packed)) change_symbol;

/**
 * @brief Data adjustment instruction layout.
 */
typedef struct _data_adjust
{
    char imm;
} __attribute__((packed)) data_adjust;

/**
 * @brief Data pointer shift instruction layout.
 */
typedef struct _data_ptr_shift
{
    unsigned short shift;
} __attribute__((packed)) data_ptr_shift;

/**
 * @brief Memory based arithmetic instruction layout
 * Adds/Subs the value at the specified location
 * to the value at the current data pointer.
 */
typedef struct _memory_arithmetic
{
    unsigned short loc;
} __attribute__((packed)) memory_arithmetic;

typedef struct _instruction
{
    op code;
    union
    {
        set_dataptr set_datap;
        branch_instr branch;
        change_symbol change_sym;
        data_adjust data_adj;
        data_ptr_shift shift_data_ptr;
        memory_arithmetic mem_arith;
    }__attribute__((packed));
} __attribute__((packed)) instruction;

// opcode handlers

opcode_status exec_data_add(VM *vm);
opcode_status exec_data_sub(VM *vm);
opcode_status exec_mem_add(VM *vm);
opcode_status exec_mem_sub(VM *vm);
opcode_status exec_data_left(VM *vm);
opcode_status exec_data_right(VM *vm);
opcode_status exec_change_symbol(VM *vm);
opcode_status exec_branch(VM *vm);
opcode_status exec_branch_eq(VM *vm);
opcode_status exec_branch_ne(VM *vm);
opcode_status exec_branch_lt(VM *vm);
opcode_status exec_branch_gt(VM *vm);
opcode_status exec_set_dataptr(VM *vm);
opcode_status exec_hlt(VM *vm);

#endif