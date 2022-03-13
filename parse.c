#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "macros.h"

expr initExpr(label_t label)
{
    // Initializes an expression with a given label.
    expr Expression;
    Expression.label = label;
    Expression.length = 0;
    Expression.atom = NULL;
    Expression.sub = NULL;
    return Expression;
}

// Parser

void printExpr(expr* Expression)
{
    // Prints the label of an expression and a summary of its contents.
    if (!Expression) {
        printf("Expression is null.\n");
    }
    printf("Label: %d\n", Expression->label);
    if (Expression->label == ATOM || Expression->label == INT || Expression->label == FLOAT || Expression->label == STRING) {
        printf("Atom contents: %s\n", Expression->atom);
    }
    else {
        printf("Number of nested expressions: %d\n", Expression->length);
    }
}

void appendExpr(expr* Expression, expr element)
{
    // Adds an expression to a list.
    Expression->atom = NULL;
    ++(Expression->length);
    Expression->sub = (expr *) realloc(Expression->sub, Expression->length * sizeof(expr));
    *(Expression->sub + Expression->length - 1) = element;
}

expr assignAtom(char* contents, int length)
{
    // Creates an atom containing the given name or literal.
    expr Expression = initExpr(ATOM);
    Expression.length = 0;
    Expression.sub = NULL;
    bool isInt = true;
    bool isFloat = false;
    Expression.atom = (char *) malloc((length + 1) * sizeof(char));
    Expression.atom[length] = 0; // Ensure null-termination
    for (int i = 0; i < length; ++i) {
        *(Expression.atom + i) = *(contents + i);
        if (('0' > *(contents + i) || *(contents + i) > '9') && !(*(contents + i) == '-' && i == 0)) {
            if (isInt && *(contents + i) == '.') {
                isFloat = true;
            }
            else {
                isFloat = false;
            }
            isInt = false;
        }
    }
    if (Expression.label != STRING) {
        if (isInt) {
            Expression.label = INT;
        }
        else if (isFloat) {
            Expression.label = FLOAT;
        }
    }
    return Expression;
}

expr *getExpr(expr* Expression, int n)
{
    // Retrieves element n from the list in the expr structure.
    if (n > Expression->length) {
        printf("Parser Error: Out of range.\n");
        exit(-1);
    }
    return (Expression->sub + n);
}

expr parse(char *code, int length, label_t label, int *cursor, macroArray *programMacros)
{
    // Recursively parses code.
    expr Expression = initExpr(label);
    int start = *cursor; // The position of the parser in the program.
    char token;
    while (*cursor < length) {
        token = *(code + *cursor); // Sets token to be the character at the cursor
        if (label == ATOM) {
            if (token == ' ' || token == ')' || token == '\n' || token == ']') {
                Expression = assignAtom(code + start, *cursor - start);
                --(*cursor);
                break;
            }
            if (*cursor == length - 1) { // Assigns an atom if it appears at the end of the code.
                Expression = assignAtom(code + start, *cursor - start + 1);
                break;
            }
        }
        else if (label == SEXPR || label == MODULE || label == ARRAY || label == MACRO) {
            if (token == ')' && label == SEXPR) { // Handles s-expressions
                break;
            }
            else if (token == ']' && label == ARRAY) {   // Handles arrays
                break;
            }
            else if (token == '}' && label == MACRO) {   // Handles macros
                macro_t newMacro;
                newMacro.keyword = getExpr(&Expression, 0)->atom;
                newMacro.exprNumber = 0;
                for (int i = 0; i < strlen(getExpr(&Expression, 1)->atom); ++i) {
                    newMacro.exprNumber *= 10;
                    newMacro.exprNumber += *(getExpr(&Expression, 1)->atom + i) - '0';
                }
                newMacro.replacement = *getExpr(&Expression, 2);
                appendMacroArray(programMacros, newMacro);
                break;
            }
            else if (token == '(') {
                ++(*cursor);
                appendExpr(&Expression, parse(code, length, SEXPR, cursor, programMacros)); // Reads s-expressions
            }
            else if (token == '[') {
                ++(*cursor);
                appendExpr(&Expression, parse(code, length, ARRAY, cursor, programMacros)); // Reads arrays
            }
            else if (token == '{') {
                ++(*cursor);
                parse(code, length, MACRO, cursor, programMacros);
            }
            else if (token == '"') {
                ++(*cursor);
                appendExpr(&Expression, parse(code, length, STRING, cursor, programMacros)); // Reads strings
            }
            else if (token == ';') {
                while (token != '\n' && *cursor < length) {
                    ++(*cursor);
                    token = *(code + *cursor);
                }
            }
            else if (token != ' ' && token != '\n' && token != '\t') {
                appendExpr(&Expression, parse(code, length, ATOM, cursor, programMacros)); // Reads other atoms
            }
        }
        else if (label == STRING) {
            if (token == '"' && *(code + *cursor - 1) != '\\') { // Finds end of string
                Expression = assignAtom(code + start, *cursor - start);
                Expression.label = STRING;
                break;
            }
        }
        ++(*cursor);
    }
    return Expression;
}

expr fullParse(char* code, int length, label_t label, int *cursor)
{
    macroArray programMacros;
    programMacros.macroNumber = 0;
    programMacros.Array = NULL;
    expr parseTree = parse(code, length, label, cursor, &programMacros);
    postParser(&parseTree, programMacros);
    return parseTree;
}