#ifndef EXEC_SEXPR_H
#define EXEC_SEXPR_H

#include "parse.h"
#include "interpret.h"

value execSexpr(expr *, scopes currentStack);

#endif