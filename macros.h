#ifndef MACROS_H
#define MACROS_H

#include "parse.h"

expr replaceMacroInstances(expr*, char*, int, expr, label_t, int*);

typedef struct {
    char *keyword;
    int exprNumber;
    expr replacement;
} macro_t;

typedef struct macroArray {
    int macroNumber;
    macro_t *Array;
} macroArray;

macroArray *newMacroArray();

void appendMacroArray(macroArray*, macro_t);

void postParser(expr*, macroArray);

void macrosDelAll(macroArray *);

#endif
