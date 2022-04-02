#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
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
    code = 0x1,
    data = 0x2,
    // Represent references that aren't resolved.
    unresolved_ind = 0x80,
    reloc_code = unresolved_ind | code,
    reloc_data = unresolved_ind | data
} label_kind;

typedef struct _label
{
    char name[256];
    label_kind kind;
    // code or data "pointer".
    short loc;
} label, *plabel;

typedef struct _line
{
    char text[256];
    instruction inst;
    // index into label table.
    int label_ref;
} line, *pline;

typedef struct _dyn_array
{
    size_t member_size;
    int populated;
    int max_entries;
    void *members;
} dyn_array, *pdyn_array;

typedef struct _parser_state
{
    dyn_array text;
    // TODO Implement embedding data in .toy file
    dyn_array data;
    dyn_array labels;
} parser_state, *pparser_state;

void init_dyn_array(pdyn_array arr, size_t member_size)
{
    arr->member_size = member_size;
    arr->populated = 0;
    arr->max_entries = 32;
    arr->members = calloc(arr->max_entries, arr->member_size);
}

void add_entry(pdyn_array arr, void *entry)
{
    if (arr->members == NULL)
    {
        return;
    }
    if (arr->populated == arr->max_entries - 1)
    {
        arr->max_entries *= 2;
        arr->members = realloc(arr->members, arr->max_entries * arr->member_size);
    }
    memcpy((char *)arr->members + (arr->populated * arr->member_size), entry, arr->member_size);
    arr->populated++;
}

void destroy_dyn_array(pdyn_array arr)
{
    free(arr->members);
    arr->members = NULL;
}

void destroy_parser(pparser_state parser)
{
    //destroy_dyn_array(&parser->data);
    destroy_dyn_array(&parser->labels);
    destroy_dyn_array(&parser->text);
}

int reference_label(pparser_state parser, int location, char *label_name, label_kind kind)
{
    label l;
    l.kind = kind;
    plabel labels = parser->labels.members;
    //printf("Label %s is at location %i\n", label_name, location);
    for (int i = 0; i < parser->labels.populated; i++)
    {
        if (!strcmp(label_name, labels[i].name))
        {
            // Label definition, set loc and clear unresolved indicator.
            if (labels[i].loc < 0 && location > 0)
            {
                labels[i].loc = location;
                labels[i].kind &= ~unresolved_ind;
            }
            // Duplicate label, return failure.
            else if (labels[i].loc > 0 && location > 0)
            {
                return -location;
            }
            return i;
        }
    }
    l.loc = location;
    strcpy(l.name, label_name);
    if (location < 0)
    {
        l.kind |= unresolved_ind;
    }
    else
    {
        l.loc = (short)location;
    }
    add_entry(&(parser->labels), &l);
    return parser->labels.populated - 1;
}

int parse_instruction(pparser_state parser, int line_index)
{
    instruction ret = {0};
    int lexemes = 0;
    char opcode[5] = {0};
    char label_ref[256] = {0};
    label_kind kind = 0;
    pline text = (pline)parser->text.members;
    sscanf(text[line_index].text, "%4s", opcode);
    if (!strcmp(opcode, "hlt"))
    {
        ret.code = hlt;
    }
    else if (!strcmp(opcode, "dl"))
    {
        unsigned short imm;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dl;
            ret.shift_data_ptr.shift = imm;
        }
    }
    else if (!strcmp(opcode, "dr"))
    {
        unsigned short imm;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dr;
            ret.shift_data_ptr.shift = imm;
        }
    }
    else if (!strcmp(opcode, "dt"))
    {
        unsigned short imm;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (lexemes == 1)
        {
            ret.code = dt;
            ret.set_tape.tape = imm;
        }
    }
    else if (!strcmp(opcode, "madd"))
    {
        unsigned short loc;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s &%256s", label_ref);
            kind = data;
        }
        else
        {
            ret.mem_arith.loc = loc;
        }
        if (lexemes == 1)
        {
            ret.code = madd;
        }
    }
    else if (!strcmp(opcode, "msub"))
    {
        unsigned short loc;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s &%256s", label_ref);
            kind = data;
        }
        else
        {
            ret.mem_arith.loc = loc;
        }
        if (lexemes == 1)
        {
            ret.code = msub;
        }
    }
    else if (!strcmp(opcode, "iadd"))
    {
        unsigned short imm = 0;
        char imm_c = 0;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c'", &imm_c);
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
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c'", &imm_c);
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
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &imm);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c'", &imm_c);
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
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s &%256s", label_ref);
            kind = code;
        }
        else
        {
            ret.branch.pos = loc;
        }
        if (lexemes == 1)
        {
            ret.code = br;
        }
    }
    else if (!strcmp(opcode, "bre"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(text[line_index].text, "%*4s %hx %hx", &imm, &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c' %hx", &imm_c, &loc);
            if (lexemes == 1)
            {
                lexemes = sscanf(text[line_index].text, "%*4s '%c' &%256s", &imm_c, label_ref);
                kind = code;
            }
            else
            {
                ret.branch.pos = loc;
            }
            imm = (unsigned short)imm_c;
        }
        else if (lexemes == 1)
        {
            lexemes = sscanf(text[line_index].text, "%*4s %hx &%256s", &imm, label_ref);
            kind = code;
        }
        else
        {
            ret.branch.pos = loc;
        }
        imm_c = (char)imm;
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = bre;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brne"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(text[line_index].text, "%*4s %hx %hx", &imm, &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c' %hx", &imm_c, &loc);
            if (lexemes == 1)
            {
                lexemes = sscanf(text[line_index].text, "%*4s '%c' &%256s", &imm_c, label_ref);
                kind = code;
            }
            else
            {
                ret.branch.pos = loc;
            }
            imm = (unsigned short)imm_c;
        }
        else if (lexemes == 1)
        {
            lexemes = sscanf(text[line_index].text, "%*4s %hx &%256s", &imm, label_ref);
            kind = code;
        }
        else
        {
            imm_c = (char)imm;
            ret.branch.pos = loc;
        }
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brne;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brlt"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(text[line_index].text, "%*4s %hx %hx", &imm, &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c' %hx", &imm_c, &loc);
            if (lexemes == 1)
            {
                lexemes = sscanf(text[line_index].text, "%*4s '%c' &%256s", &imm_c, label_ref);
                kind = code;
            }
            else
            {
                ret.branch.pos = loc;
            }
            imm = (unsigned short)imm_c;
        }
        else if (lexemes == 1)
        {
            lexemes = sscanf(text[line_index].text, "%*4s %hx &%256s", &imm, label_ref);
            kind = code;
        }
        else
        {
            imm_c = (char)imm;
            ret.branch.pos = loc;
        }
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brlt;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "brgt"))
    {
        unsigned short imm, loc;
        char imm_c;
        lexemes = sscanf(text[line_index].text, "%*4s %hx %hx", &imm, &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s '%c' %hx", &imm_c, &loc);
            if (lexemes == 1)
            {
                lexemes = sscanf(text[line_index].text, "%*4s '%c' &%256s", &imm_c, label_ref);
                kind = code;
            }
            else
            {
                ret.branch.pos = loc;
            }
            imm = (unsigned short)imm_c;
        }
        else if (lexemes == 1)
        {
            lexemes = sscanf(text[line_index].text, "%*4s %hx &%256s", &imm, label_ref);
            kind = code;
        }
        else
        {
            imm_c = (char)imm;
            ret.branch.pos = loc;
        }
        if (lexemes == 2 && imm == imm_c)
        {
            ret.code = brgt;
            ret.branch.val = imm_c;
        }
    }
    else if (!strcmp(opcode, "setd"))
    {
        unsigned short loc;
        lexemes = sscanf(text[line_index].text, "%*4s %hx", &loc);
        if (!lexemes)
        {
            lexemes = sscanf(text[line_index].text, "%*4s &%256s", label_ref);
            kind = data;
        }
        else
        {
            ret.set_datap.pos = loc;
        }
        if (lexemes == 1)
        {
            ret.code = setd;
        }
    }
    if (ret.code != 0)
    {
        text[line_index].inst = ret;
        if (label_ref[0])
        {
            text[line_index].label_ref = reference_label(parser, -line_index, label_ref, kind);
        }
        return line_index;
    }
    return -line_index;
}

int parse_line(pparser_state parser, char *line)
{
    int ret = 0;
    char label_name[256] = {0};
    int lexemes = sscanf(line, ":%256s\n", label_name);
    // : at the start of a line is a label
    if (lexemes)
    {
        ret = reference_label(parser, parser->text.populated, label_name, code);
    }
    else
    {
        struct _line l = {0};
        // Assume no label reference by default.
        l.label_ref = -1;
        strcpy(l.text, line);
        add_entry(&parser->text, &l);
        ret = parse_instruction(parser, parser->text.populated - 1);
    }
    return ret;
}

int relocate_instruction(pparser_state parser, int line_index)
{
    pline text = parser->text.members;
    plabel labels = parser->labels.members;
    line lin = text[line_index];
    label lab = labels[lin.label_ref];
    switch ((char)lin.inst.code)
    {
    case (char)br:
    case (char)bre:
    case (char)brne:
    case (char)brlt:
    case (char)brgt:
        if (lab.kind != code)
        {
            return -line_index;
        }
        text[line_index].inst.branch.pos = lab.loc;
        return line_index;
    case (char)madd:
    case (char)msub:
        if (lab.kind != data)
        {
            return -line_index;
        }
        text[line_index].inst.mem_arith.loc = lab.loc;
        return line_index;
    case (char)setd:

        if (lab.kind != data)
        {
            return -line_index;
        }
        text[line_index].inst.set_datap.pos = lab.loc;
        return line_index;
    default:
        return -line_index;
    }
    return line_index;
}

void init_parser_state(pparser_state parser)
{
    init_dyn_array(&(parser->labels), sizeof(label));
    init_dyn_array(&(parser->text), sizeof(struct _line));
    //init_dyn_array(&(parser->data), sizeof(char));
}

int parse_file(FILE *fp, program toy_bin)
{
    int i = 0;
    int valid = 1;
    char line[256] = {0};
    parser_state parser = {0};
    init_parser_state(&parser);
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        valid = parse_line(&parser, line);
        if (valid < 0)
        {
            destroy_parser(&parser);
            return valid;
        }
    }
    pline text = parser.text.members;
    for (; i < parser.text.populated; i++)
    {
        if (text[i].label_ref >= 0)
        {
            valid = relocate_instruction(&parser, i);
            if (valid < 0)
            {
                destroy_parser(&parser);
                return valid;
            }
        }
        toy_bin[i] = text[i].inst;
    }
    destroy_parser(&parser);
    return i;
}