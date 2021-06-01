#ifndef OPCODES_H
#define OPCODES_H

typedef enum _opcode
{
    dadd = 0xa0,
    dsub = 0xa1,
    madd = 0x10,
    msub = 0x11,
    dl = 0xd0,
    dr = 0xd1,
    cs = 0xc0,
    br = 0xb0,
    bre = 0xb1,
    brne = 0xb2,
    brlt = 0xb3,
    brgt = 0xb4,
    setd = 0x50,
    hlt = 0xff
} opcode;

#endif