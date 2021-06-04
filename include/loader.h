#ifndef LOADER_H
#define LOADER_H

/**
 * @brief Describes the header of a toy binary
 * The program follows immediately after, with
 * any preconfigured data appended to that.
 */
typedef struct _toy_file
{
    int program_len;
    int data_len;
} toy_file;

#endif // LOADER_H