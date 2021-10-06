#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "hash_table.h"
#include "parser.h"
#include "opcodes_internal.h"
#include "opcodes.h"

//char permut[] = {186, 35, 99, 61, 48, 140, 174, 32, 8, 153, 23, 11, 116, 89, 109, 252, 66, 20, 71, 205, 22, 141, 177, 124, 216, 15, 214, 28, 129, 74, 175, 207, 204, 151, 24, 133, 126, 67, 43, 106, 73, 197, 209, 60, 81, 219, 113, 118, 187, 77, 155, 192, 227, 157, 83, 29, 30, 152, 53, 222, 203, 3, 188, 117, 114, 162, 0, 230, 27, 208, 161, 39, 149, 145, 200, 6, 137, 19, 238, 196, 235, 170, 41, 18, 211, 16, 105, 255, 229, 150, 241, 218, 236, 93, 5, 98, 194, 249, 112, 154, 49, 210, 195, 169, 57, 78, 70, 171, 55, 231, 131, 179, 92, 33, 217, 96, 12, 9, 223, 148, 121, 76, 64, 100, 90, 245, 103, 234, 224, 75, 86, 165, 180, 239, 111, 225, 104, 215, 139, 183, 115, 87, 250, 181, 144, 156, 52, 107, 142, 130, 82, 14, 240, 213, 244, 221, 47, 128, 125, 226, 2, 119, 25, 97, 191, 198, 251, 59, 10, 108, 95, 189, 26, 122, 164, 138, 58, 1, 206, 54, 62, 228, 167, 79, 159, 143, 4, 46, 31, 72, 201, 132, 123, 193, 163, 136, 135, 212, 94, 254, 42, 190, 173, 178, 21, 166, 63, 80, 172, 34, 242, 158, 146, 247, 176, 253, 147, 44, 110, 36, 65, 45, 233, 101, 220, 84, 160, 68, 37, 85, 102, 40, 17, 237, 185, 88, 243, 13, 246, 7, 199, 184, 202, 69, 182, 91, 232, 56, 248, 168, 120, 51, 50, 127, 38, 134};
//
//int pearson_hash(void *elem)
//{
//    int h = 0;
//    return h;
//}

typedef enum _label_kind
{
    code = 1,
    data = 2,
    // Represent references that aren't resolved.
    reloc_ind = 0x10,
    reloc_code = reloc_ind | code,
    reloc_data = reloc_ind | data
} label_kind;

typedef struct _label
{
    char name[255];
    label_kind kind;
    // code or data "pointer".
    short loc;
} label, *plabel;

typedef struct _line
{
    char text[255];
    instruction inst;
    // index into label table.
    int label_ref;
} line, *pline;

typedef struct _parser_state
{
    short text_size;
    short max_text_size;
    pline text;
    // TODO Implement embedding data in .toy file
    short data_size;
    short max_data_size;
    char *data;
    int label_cnt;
    int max_label_cnt;
    plabel labels;
} parser_state, *pparser_state;

instruction parse_line(pparser_state parse, char *line)
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
            ret.shift_data_ptr.shift = imm;
        }
    }
    else if (!strcmp(opcode, "dr"))
    {
        unsigned short imm;
        lexemes = sscanf(line, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dr;
            ret.shift_data_ptr.shift = imm;
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c'", &imm_c);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c'", &imm_c);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c'", &imm_c);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c' %hx", &imm_c, &loc);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c' %hx", &imm_c, &loc);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c' %hx", &imm_c, &loc);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
        if (!lexemes)
        {
            lexemes = sscanf(line, "%*4s '%c' %hx", &imm_c, &loc);
            imm = (unsigned short)imm_c;
        }
        else
        {
            imm_c = (char)imm;
        }
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
    parser_state parser = {0};
    for (; i <= MAX_PROGRAM_SIZE; i++)
    {
        if (fgets(line, sizeof(line), fp) != NULL)
        {
            toy_bin[i] = parse_line(&parser, line);
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