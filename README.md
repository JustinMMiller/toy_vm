# Overview

Each instruction is 4 bytes, each cell is 1 byte.
Instructions are kept in a separate location from data.

The following is a sample program that changes the second cell to 0x10
and halts.
```c
cs 0x10
:LoopBegin
setd 0x0
bre 0x0 &LoopEnd
isub 0x1
setd 0x1
iadd 0x1
br &LoopBegin
:LoopEnd
hlt
```

# Instructions

## hlt
Halts execution of the program.

## dl/dr \<n\>
Shifts the data pointer left/right by *n* cells

## dt \<n\>
Switch to tape n.

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
