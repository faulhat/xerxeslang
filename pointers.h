#ifndef POINTERS_H
#define POINTERS_H

#include "interpret.h"

value retriveInt(value);

value retrieveFloat(value);

value retrieveString(value);

value getAddressOf(variable*);

value retrieveWithCommand(value, char*);

void reassignAddress(value, value);

#endif