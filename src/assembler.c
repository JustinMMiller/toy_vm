#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "parser.h"
#include "opcodes_internal.h"

int main(int argc, char **argv)
{
    FILE *finput, *foutput;
    program toy = NULL;
    toy_file toy_bin = {0};
    int instructions_parsed;
    if (argc != 3)
    {
        printf("assembler takes exactly 2 arguments!\n");
        printf("Usage : assembler <input.toy> <output.bin>\n");
        return __LINE__;
    }
    finput = fopen(argv[1], "r");
    foutput = fopen(argv[2], "w");
    if (!finput)
    {
        printf("Unable to open %s for reading!\n", argv[1]);
        return __LINE__;
    }
    if (!foutput)
    {
        printf("Unable to open %s for writing!\n", argv[2]);
        return __LINE__;
    }
    toy = (program)calloc(MAX_PROGRAM_SIZE + 1, sizeof(instruction));
    if (!toy)
    {
        printf("Insufficient resources for assembler\n");
        return __LINE__;
    }
    instructions_parsed = parse_file(finput, toy);
    if (instructions_parsed > 0)
    {
        toy_bin.program_len = instructions_parsed;
        fwrite(&toy_bin, sizeof(toy_bin), 1, foutput);
        fwrite(toy, instructions_parsed, sizeof(instruction), foutput);
    }
    else
    {
        printf("Instruction %i failed to parse!\n", instructions_parsed);
        return __LINE__;
    }
    fclose(finput);
    fclose(foutput);
    return instructions_parsed < 0 ? instructions_parsed : 0;
}