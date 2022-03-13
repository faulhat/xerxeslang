#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "parse.h"
#include "interpret.h"

void treeWalk(expr *Expression, int depth)
{
    // Walks the expression tree generated by the parser; for debugging use
    for (int i = 0; i < depth; ++i) {
        printf("-");
    }
    if (Expression->label == ATOM || Expression->label == INT || Expression->label == FLOAT || Expression->label == STRING) {
        printf("Atom contents: %s.\n", Expression->atom);
    }
    else {
        printf("Number of nested expressions: %d.\n", Expression->length);
        for (int i = 0; i < Expression->length; ++i) {
            treeWalk(getExpr(Expression, i), depth + 1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Not enough command line args! Need two, got %d.\n", argc);
        return -1;
    }

    FILE *stream = fopen(argv[1], "r");
    if (!stream) {
        printf("Error: no such file.\n");
        exit(-1);
    }
    char *buffer = NULL;
    size_t len = 0;
    ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);

    int end = 0;
    expr parsedProgram = fullParse(buffer, strlen(buffer), MODULE, &end);

    free(buffer);
    scopes mainScopeStack = initScopes();
    value programEval = execExprList(&parsedProgram, NULL, 0, mainScopeStack, NULL);

    return 0;
}