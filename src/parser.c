#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "opcodes_internal.h"
#include "opcodes.h"

instruction parse_line(char *line)
{
    instruction ret = {0};
    int lexemes = 0;
    char opcode[5] = {0};
    sscanf(line, "%4s", opcode);
    if (!strcmp(opcode, "hlt"))
    {
        ret.code = hlt;
    }
    else if (!strcmp(opcode, "dl"))
    {
        unsigned short imm;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dl;
            ret.data_adj.imm = imm;
        }
    }
    else if (!strcmp(opcode, "dr"))
    {
        unsigned short imm;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dr;
            ret.data_adj.imm = imm;
        }
    }
    else if (!strcmp(opcode, "madd"))
    {
        unsigned short loc;
        lexemes = sscanf(line, "%*4s %hx", &loc);
        if (lexemes == 1)
        {
            ret.code = madd;
            ret.mem_arith.loc = loc;
        }
    }
    else if (!strcmp(opcode, "msub"))
    {
        unsigned short loc;
        lexemes = sscanf(line, "%*4s %hx", &loc);
        if (lexemes == 1)
        {
            ret.code = msub;
            ret.mem_arith.loc = loc;
        }
    }
    else if (!strcmp(opcode, "iadd"))
    {
        unsigned short imm = 0;
        char imm_c = 0;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        imm_c = (char)imm;
        // If the immediate was not a char, we will return 0
        // to indicate error.
        if ((unsigned short)imm_c == imm && lexemes == 1)
        {
            ret.code = iadd;
            ret.data_adj.imm = imm_c;
        }
    }
    else if (!strcmp(opcode, "isub"))
    {
        unsigned short imm;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        imm_c = (char)imm;
        // If the immediate was not a char, we will return 0
        // to indicate error.
        if (imm_c == imm && lexemes == 1)
        {
            ret.code = isub;
            ret.data_adj.imm = imm_c;
        }
    }
    else if (!strcmp(opcode, "cs"))
    {
        unsigned short imm;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        imm_c = (char)imm;
        // If the immediate was not a char, we will return 0
        // to indicate error.
        if (imm_c == imm && lexemes == 1)
        {
            ret.code = cs;
            ret.change_sym.sym = imm_c;
        }
    }
    else if (!strcmp(opcode, "br"))
    {
        unsigned short loc;
        lexemes = sscanf(line, "%*4s %hx", &loc);
        if (lexemes == 1)
        {
            ret.code = br;
            ret.branch.pos = loc;
        }
    }
    else if (!strcmp(opcode, "bre"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx %hx", &imm, &loc);
        imm_c = (char)imm;
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = bre;
            ret.branch.pos = loc;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brne"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx %hx", &imm, &loc);
        imm_c = (char)imm;
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brne;
            ret.branch.pos = loc;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brlt"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx %hx", &imm, &loc);
        imm_c = (char)imm;
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brlt;
            ret.branch.pos = loc;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brgt"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(line, "%*4s %hx %hx", &imm, &loc);
        imm_c = (char)imm;
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brgt;
            ret.branch.pos = loc;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "setd"))
    {
        unsigned short loc;
        lexemes = sscanf(line, "%*4s %hx", &loc);
        if (lexemes == 1)
        {
            ret.code = setd;
            ret.set_datap.pos = loc;
        }
    }
    return ret;
}

int parse_file(FILE *fp, program toy_bin)
{
    int i = 0;
    char line[255] = {0};
    for (; i <= MAX_PROGRAM_SIZE; i++)
    {
        if (fgets(line, sizeof(line), fp) != NULL)
        {
            toy_bin[i] = parse_line(line);
            if (toy_bin[i].code == 0)
            {
                i = -(i + 1);
                break;
            }
        }
        else
        {
            break;
        }
    }
    // if fgets returns null on the first run,
    // return -1 to indicate failure on the first
    // instruction.
    return i == 0 ? -1 : i;
}