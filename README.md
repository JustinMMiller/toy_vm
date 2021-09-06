# Overview

Each instruction is 4 bytes, each cell is 1 byte.
Instructions are kept on a separate tape from data.

# Instructions

## hlt
Halts execution of the program.

## dl/dr \<n\>
Shifts the data pointer left/right by *n* cells

## madd/msub \<m>
Performs the operation *data_pointer -= / += *data[m]

## iadd/isub \<i>
Performs the operation *data_pointer -= / += \<i>

\<i> may be represented as either a hexadecimal value (0xa) or a character ('a')

## cs \<s>
Sets the current data cell to the specified symbol (either hex or character)

## br \<l>
Unconditional branch to location \<l>

## br{n}e \<s> \<l>
Branch to location \<l> if the current data cell is {not} equal to the specified symbol \<s>.

## br{l|g}t \<s> \<l>
Branch to location \<l> if the current data cell is less/greater than the specified symbol \<s>.

## setd \<d>
Set the data pointer to the specified data location \<d>