#ifndef FILE_OPEN_H
#define FILE_OPEN_H

#include "pointers.h"

value fileOpenBinary(value, value);

value fileOpenText(value, value);

value writeToFile(value, value);

value writeToFileInt(value, value);

value readFromFile(value, value);

value readFromFile(value, value);

value readFromFileDelim(value, value);

#endif