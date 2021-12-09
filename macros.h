#include "proto_2.h"

expr replaceMacroInstances(expr*, char*, int, expr, int, int*);

typedef struct macro_s {
	char *keyword;
	int exprNumber;
	expr replacement;
} macro_t;

typedef struct macroArray {
	int macroNumber;
	macro_t *Array;
} macroArray;

void appendMacroArray(macroArray*, macro_t);

void postParser(expr*, macroArray);