#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"

expr *getExprMac(expr* Expression, int n, char *macroName)
{
    // Retrieves element n from the list in the expr structure.
    if (n > Expression->length) {
        printf("Parser Error: Macro '%s' Failure.\n", macroName);
        exit(-1);
    }
    return (Expression->sub + n);
}

expr deepCopy(expr *code, label_t label)
{
    expr newExpr = initExpr(label);
    for (int i = 0; i < code->length; ++i) {
        label_t label = code->sub[i].label;
        if (label != ATOM && label != INT && label != FLOAT) {
            appendExpr(&newExpr, deepCopy(code->sub + i, label));
        }
        else {
            appendExpr(&newExpr, initExpr(label));
            newExpr.sub[i].atom = (char *) malloc((strlen(code->sub[i].atom) + 1) * sizeof(char));
            strcpy(newExpr.sub[i].atom, code->sub[i].atom);
        }
    }
    
    return newExpr;
}

void findAndReplace(expr *code, char *atomFind, expr replacement)
{
    for (int i = 0; i < code->length; ++i) {
        expr thisExpr = code->sub[i];
        if (thisExpr.label == MACRO) {
            continue;
        }

        if (thisExpr.label == ATOM) {
            if (strcmp(thisExpr.atom, atomFind) == 0) {
                code->sub[i] = replacement;
            }
        }
        else if (thisExpr.label != INT && thisExpr.label != FLOAT) {
            findAndReplace(code->sub + i, atomFind, replacement);
        }
    }
}

expr replaceMacroInstances(expr *code, char *macroFind, int macroExprs, expr macroReplace, label_t label, int *instances)
{
    *instances = 0;
    expr newCode = initExpr(label);
    expr *replacementCode = (expr *) malloc(sizeof(expr));
    for (int i = 0; i < code->length; i++) {
        expr thisExpr = code->sub[i];
        if (thisExpr.label != SEXPR) {
            appendExpr(&newCode, thisExpr);
            continue;
        }

        if (strcmp((thisExpr.sub)->atom, macroFind) == 0) {
            *replacementCode = deepCopy(&macroReplace, macroReplace.label);
            for (int k = 0; k < macroExprs; ++k) {
                char *exprISign = (char *) calloc(20, sizeof(char));
                exprISign[0] = '%';
                sprintf(exprISign + 1, "%d", k);
                expr exprI = *getExprMac(&thisExpr, k + 1, macroFind);
                findAndReplace(replacementCode, exprISign, exprI);
            }
            
            (*instances)++;
            appendExpr(&newCode, *replacementCode);
        }
        else {
            appendExpr(&newCode, replaceMacroInstances(&thisExpr, macroFind, macroExprs, macroReplace, SEXPR, instances));
        }
    }

    return newCode;
}


macroArray *newMacroArray()
{
    macroArray *array_p = (macroArray *) malloc(sizeof(macroArray));
    array_p->macroNumber = 0;
    array_p->Array = (macro_t *) malloc(0);

    return array_p;
}

void appendMacroArray(macroArray *macros, macro_t newMacro)
{
    macros->macroNumber++;
    macros->Array = (macro_t *) realloc(macros->Array, macros->macroNumber * sizeof(macro_t));
    macros->Array[macros->macroNumber - 1] = newMacro;
}

void postParser(expr *fullCode, macroArray macros)
{
    for (int j = 0; j < macros.macroNumber; ++j) {
        macro_t thisMacro = *(macros.Array + j);
        int i = 1;
        while (i > 0) {
            *fullCode = replaceMacroInstances(fullCode, thisMacro.keyword, thisMacro.exprNumber, thisMacro.replacement, MODULE, &i);
        }
    }
}

void macrosDelAll(macroArray *toDel)
{
    for (int i = 0; i < toDel->macroNumber; i++) {
        free(toDel->Array[i].keyword);
    }

    free(toDel->Array);
    free(toDel);
}





