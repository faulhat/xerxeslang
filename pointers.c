#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "interpret.h"
#include "macros.h"

value retrieveInt(value address)
{
    value *intPtr = (value *) address.Gen.LongInt;
    value thisInt = initValue();
    thisInt.type = INT;
    thisInt.Gen.Integer = intPtr->Gen.Integer;
    return thisInt;
}

value retrieveFloat(value address)
{
    value *floatPtr = (value *) address.Gen.LongInt;
    value thisFloat = initValue();
    thisFloat.type = FLOAT;
    thisFloat.Gen.Float = floatPtr->Gen.Float;
    return thisFloat;
}

value retrieveString(value address)
{
    value *stringPtr = (value *) address.Gen.LongInt;
    value thisString = initValue();
    thisString.type = STRING;
    thisString.Gen.String = stringPtr->Gen.String;
    return thisString;
}

value retrieveArray(value address)
{
    value *arrayPtr = (value *) address.Gen.LongInt;
    value thisArray = initValue();
    thisArray.type = ARRAY;
    thisArray.arrayLength = arrayPtr->arrayLength;
    thisArray.Array = arrayPtr->Array;
    return thisArray;
}

value getAddressOf(variable *item)
{
    value address = initValue();
    address.type = ADDRESS;
    address.Gen.LongInt = (long) &(item->Var.Value);
    return address;
}

value retrieveWithCommand(value address, char *command)
{
    if (strcmp(command, "Int*") == 0) {
        return retrieveInt(address);
    }
    else if (strcmp(command, "Float*") == 0) {
        return retrieveFloat(address);
    }
    else if (strcmp(command, "String*") == 0) {
        return retrieveString(address);
    }
    else if (strcmp(command, "Array*") == 0) {
        return retrieveArray(address);
    }
    else {
        printf("Pointer Error: Invalid address retrieval command.\n");
        exit(-1);
    }
}

void reassignAddress(value address, value newValue)
{
    value *address_i = (value *) address.Gen.LongInt;
    *address_i = newValue;
}