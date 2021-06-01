#ifndef OPCODES_H
#define OPCODES_H

typedef enum _opcode
{
    hlt = 0xff,
    dl = 0xd0,
    dr = 0xd1,
    madd = 0x10,
    msub = 0x11,
    iadd = 0xe0,
    isub = 0xe1,
    cs = 0xc0,
    br = 0xb0,
    bre = 0xb1,
    brne = 0xb2,
    brlt = 0xb3,
    brgt = 0xb4,
    setd = 0x50,
} opcode;

#endif