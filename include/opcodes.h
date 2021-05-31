#ifndef OPCODES_H
#define OPCODES_H

typedef enum _opcode
{
    dl = 0xd0,
    dr = 0xd1,
    cs = 0xc0,
    bre = 0xb0,
    brne = 0xb1,
    brlt = 0xb2,
    brgt = 0xb3,
    hlt = 0xff
} opcode;

#endif