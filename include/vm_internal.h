#ifndef VM_INTERNAL_H
#define VM_INTERNAL_H

typedef enum _opcode_status
{
    successful,
    exc_left_edge,
    exc_right_edge,
    exc_invalid_tape,
    exc_last_instr
} opcode_status;

#endif
