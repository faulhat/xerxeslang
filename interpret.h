#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdio.h>
#include <stdint.h>

#include "parse.h"
#include "map.h"

char *getPrefix(char *);

value typecast(value, char *);

value bitwiseWithCommand(value, value, char *);

value execute(expr *, scopes);

value execExprList(expr*, variable*, int, scopes, map*);

#endif