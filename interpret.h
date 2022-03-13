#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdio.h>
#include <stdint.h>

#include "parse.h"
#include "map.h"

value execExprList(expr*, variable*, int, scopes, map*);

#endif