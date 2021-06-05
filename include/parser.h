#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "vm.h"

/**
 * @brief Parse the given file into the given program
 * @return The number of instructions parsed on success,
 *         the negative index of the failed instruction
 *         on failure.
 */
int parse_file(FILE *, program);

#endif // PARSER_H