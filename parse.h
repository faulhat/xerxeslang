#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

// Labels for types of expressions. List types are multiples of 10, so that
// whether something is a list or an atom can be tested by using the modulus function.
typedef enum {
    ATOM,
    LIST,
    INT,
    FLOAT,
    STRING,
    SEXPR,
    MODULE,
    FUNCTION,
    ARRAY,
    COMMENT,
    LIBRARY,
    MACRO,
    ADDRESS,
    SYS_FILE
} label_t;

// A structure for storing expressions and expression trees.
typedef struct expr {
    label_t label; // What kind of expression is this (list or atom)?
    int length; // The number of nested expressions this list contains (if it is a list).
    char *atom; // The contents of the atom.
    struct expr *sub; // The expressions within this list.
} expr;

expr initExpr(label_t);

expr *getExpr(expr *, int);

typedef struct macroArray macroArray; // Forward declaration
expr parse(char *, int, label_t, int *, macroArray *);

expr fullParse(char *, int, label_t, int*);

void appendExpr(expr *, expr);

#endif
