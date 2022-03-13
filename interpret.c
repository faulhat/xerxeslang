#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "parse.h"
#include "interpret.h"
#include "file_open.h"
#include "pointers.h"
#include "macros.h"
#include "map.h"
#include "exec_sexpr.h"
#include "smart_str.h"

const uint32_t MAP_INIT_HASHLEN = 6;

// Interpreter (Prototype 2, can handle multiple types)

value typecast(value source, char *cast)
{
    // Casts a "value" from one type to another.
    value returnVal = initValue();
    if (source.type == STRING) {
        if (strcmp(cast, "String") == 0) {
            return source;
        }

        printf("Type Error: cannot coerce string to type %s\n", cast);
        exit(-1);
    }
    else if (source.type == INT) {
        if (strcmp(cast, "Int") == 0) {
            return source;
        }
        else if (strcmp(cast, "Float") == 0) {
            returnVal.type = FLOAT;
            returnVal.Gen.Float = (double) source.Gen.Integer;
            return returnVal;
        }
        else if (strcmp(cast, "String") == 0) {
            returnVal.type = STRING;
            char *buffer = (char *) malloc(20 * sizeof(char));
            sprintf(buffer, "%d", source.Gen.Integer);
            returnVal.Gen.String = buffer;
            return returnVal;
        }
    }
    else if (source.type == FLOAT) {
        if (strcmp(cast, "Float") == 0) {
            return source;
        }
        else if (strcmp(cast, "Int") == 0) {
            returnVal.type = INT;
            returnVal.Gen.Integer = (int) source.Gen.Float;
            return returnVal;
        }
        else if (strcmp(cast, "String") == 0) {
            returnVal.type = STRING;
            char *buffer = (char *) malloc(40 * sizeof(char));
            sprintf(buffer, "%f", source.Gen.Float);
            returnVal.Gen.String = buffer;
            return returnVal;
        }
    }
    printf("Error: Invalid cast.\n");
    exit(-1);
}

int getPrefix(char *source)
{
    // Finds everything in a name up to a dot.
    // Returns the number of characters before that dot.
    for (int i = 0; i < strlen(source); ++i) {
        if (*(source + i) == '.') {
            return i;
        }
    }
    return 0; // Returns 0 if no dot is found.
}

value bitwiseWithCommand(value valueOperand1, value valueOperand2, char *command)
{
    int operand1 = valueOperand1.Gen.Integer;
    int operand2 = valueOperand2.Gen.Integer;
    value result = initValue();
    result.type = INT;
    if (strcmp(command, "^") == 0) {
        result.Gen.Integer = (operand1 ^ operand2);
    }
    else if (strcmp(command, "|") == 0) {
        result.Gen.Integer = (operand1 | operand2);
    }
    else if (strcmp(command, "&") == 0) {
        result.Gen.Integer = (operand1 & operand2);
    }
    else if (strcmp(command, ">>") == 0) {
        result.Gen.Integer = (operand1 >> operand2);
    }
    else if (strcmp(command, "<<") == 0) {
        result.Gen.Integer = (operand1 << operand2);
    }
    else {
        printf("Error: Invalid bitwise operation.");
        exit(-1);
    }
    return result;
}

// Forward declaration of execExprList for use in execute.
value execExprList(expr *exprList, variable *arguments, int numberOfArguments, scopes allHigherScopes, map *ExportTo);

value execute(expr *Expression, scopes currentStack)
{
    // Executes single s-expressions and atoms.
    if (Expression->label == ATOM) { // Default ATOM label indicates a word
        return getVariable(currentStack, Expression->atom)->Var.Value;
    }
    else if (Expression->label == INT) {   // Calculates the value of an int literal.
        value returnVal = initValue();
        returnVal.type = INT;
        int i = 0;
        int p = 1;
        if (*(Expression->atom) == '-') {
            i = 1;
            p = -1;
        }
        for (; i < strlen(Expression->atom); ++i) {
            returnVal.Gen.Integer *= 10;
            returnVal.Gen.Integer += *(Expression->atom + i) - '0';
        }
        returnVal.Gen.Integer *= p;
        return returnVal;
    }
    else if (Expression->label == FLOAT) {   // Calculates the value of a float literal.
        value returnVal = initValue();
        returnVal.type = FLOAT;
        int dot;
        for (int i = 0; i < strlen(Expression->atom); ++i) {
            if (*(Expression->atom + i) == '.') {
                dot = i;
            }
        }
        int i = 0;
        int p = 1;
        if (*(Expression->atom) == '-') {
            i = 1;
            p = -1;
        }
        for (; i < dot; ++i) {
            returnVal.Gen.Float *= 10.0;
            returnVal.Gen.Float += (double) *(Expression->atom + i) - '0';
        }
        i = dot + 1;
        for (; i < strlen(Expression->atom); ++i) {
            double place = 1.0;
            for (int j = 0; j < i - dot; j++) {
                place *= 0.1;
            }
            returnVal.Gen.Float += place * ((double) *(Expression->atom + i) - '0');
        }
        returnVal.Gen.Float *= p;
        return returnVal;
    }
    else if (Expression->label == STRING) {
        value returnVal = initValue();
        returnVal.type = STRING;
        char *temp = Expression->atom;

        smart_str_t *buf = emptyStr();
        for (int i = 0; i < strlen(temp); i++) {
            char thisCharacter = temp[i];
            if (thisCharacter == '\\') {
                if (temp[i + 1] == '"') {
                    str_putc(buf, '"');
                }
                else if (temp[i + 1] == 'n') {
                    str_putc(buf, '\n');
                }
                else if (temp[i + 1] == 't') {
                    str_putc(buf, '\t');
                }
                else if (temp[i + 1] == '\\') {
                    str_putc(buf, '\\');
                }
                else {
                    printf("Error: Unrecognized escape sequence.\n");
                    exit(-1);
                }
                
                i++;
            }
            else {
                str_putc(buf, thisCharacter);
            }
        }

        returnVal.Gen.String = getContentsCpy(buf);
        return returnVal;
    }
    else if (Expression->label == ARRAY) {
        value returnVal = initValue();
        returnVal.type = ARRAY;
        returnVal.Array = (value *) malloc(Expression->length * sizeof(value));
        for (int i = 0; i < Expression->length; ++i) {
            *(returnVal.Array + i) = execute(getExpr(Expression, i), currentStack);
            ++(returnVal.arrayLength);
        }
        return returnVal;
    }
    else if (Expression->label == SEXPR) {
        return execSexpr(Expression, currentStack);
    }

    return initValue();
}


value execExprList(expr *exprList, variable *arguments, int numberOfArguments, scopes allHigherScopes, map *ExportTo)
{
    // Executes lists of s-expressions and atoms, such as modules and functions.
    scopes *currentStack = (scopes *) malloc((allHigherScopes.depth + 1) * sizeof(scopes));
    *currentStack = plusScope(allHigherScopes, initMap());
    for (int i = 0; i < numberOfArguments; ++i) {
        newVariable((currentStack->stack), *(arguments + i));
    }
    expr *exprI = (expr *) malloc(sizeof(expr));
    expr *first = (expr *) malloc(sizeof(expr));
    value lineValue = initValue();
    for (int i = 0; i < exprList->length; ++i) {
        *exprI = *getExpr(exprList, i);
        if (exprI->label == ATOM || exprI->label == INT || exprI->label == FLOAT || exprI->label == STRING) {
            lineValue = execute(exprI, *currentStack);
        }
        else if (exprI->label == SEXPR) {
            *first = *getExpr(exprI, 0);
            if (strcmp(first->atom, "define") == 0) {
                variable newDefinition;
                if (getExpr(exprI, 1)->label != ATOM) {
                    printf("Syntax error: variable name cannot be %s.\n", getExpr(exprI, 1)->atom);
                    exit(-1);
                }
                newDefinition.name = getExpr(exprI, 1)->atom;
                newDefinition.Var.Value = execute(getExpr(exprI, 2), *currentStack);
                newVariable(currentStack->stack, newDefinition);
                lineValue = initValue();
            }
            else if (strcmp(first->atom, ":") == 0) {
                if (getExpr(exprI, 1)->label != ATOM) {
                    if (getExpr(exprI, 1)->label == SEXPR) {
                        if (strcmp(getExpr(getExpr(exprI, 1), 0)->atom, "get") == 0) {
                            char *targetArrayName = getExpr(getExpr(exprI, 1), 1)->atom;
                            value targetArray = getVariable(*currentStack, targetArrayName)->Var.Value;
                            value n = execute(getExpr(getExpr(exprI, 1), 2), *currentStack);
                            if (targetArray.type == ARRAY) {
                                *(targetArray.Array + n.Gen.Integer) = execute(getExpr(exprI, 2), *currentStack);
                            }
                            else if (targetArray.type == STRING) {
                                *(targetArray.Gen.String + n.Gen.Integer) = *(execute(getExpr(exprI, 2), *currentStack).Gen.String);
                            }
                        }
                        else {
                            printf("Error: invalid syntax in assignment.\n:");
                            exit(-1);
                        }
                    }
                    else {
                        printf("Error: invalid syntax in assignment.\n:");
                        exit(-1);
                    }
                }
                else {
                    char *targetVar = getExpr(exprI, 1)->atom;
                    getVariable(*currentStack, targetVar)->Var.Value = execute(getExpr(exprI, 2), *currentStack);
                }
                lineValue = initValue();
            }
            else if (strcmp(first->atom, "function") == 0) {
                variable newFunction = initVariable();
                expr *declaration = getExpr(exprI, 1);
                if (getExpr(declaration, 0)->label != ATOM) {
                    printf("Syntax error: function name cannot be %s.\n", getExpr(declaration, 1)->atom);
                    exit(-1);
                }
                newFunction.name = getExpr(declaration, 0)->atom;
                newFunction.Var.Function.argumentNames = (char **) malloc((declaration->length - 1) * sizeof(char *));
                newFunction.Var.Function.argumentNumber = declaration->length - 1;
                for (int j = 1; j < declaration->length; ++j) {
                    if (getExpr(declaration, j)->label != ATOM) {
                        printf("Syntax error: %s is an invalid argument name.\n", getExpr(declaration, j)->atom);
                        exit(-1);
                    }
                    *(newFunction.Var.Function.argumentNames + j - 1) = getExpr(declaration, j)->atom;
                }
                newFunction.Var.Function.body = initExpr(FUNCTION);
                for (int j = 2; j < exprI->length; ++j) {
                    appendExpr(&(newFunction.Var.Function.body), *getExpr(exprI, j));
                }
                newVariable(currentStack->stack, newFunction);
                lineValue = initValue();
            }
            else if (strcmp(first->atom, "if") == 0) {
                if (execute(getExpr(exprI, 1), *currentStack).Gen.Integer != 0 || execute(getExpr(exprI, 1), *currentStack).Gen.Float != 0.0) {
                    expr iftrue = initExpr(SEXPR);
                    appendExpr(&iftrue, *getExpr(exprI, 2));
                    lineValue = execExprList(&iftrue, NULL, 0, *currentStack, NULL);
                }
                else {
                    if (exprI->length == 4) {
                        expr iffalse = initExpr(SEXPR);
                        appendExpr(&iffalse, *getExpr(exprI, 3));
                        lineValue = execExprList(&iffalse, NULL, 0, *currentStack, NULL);
                    }
                }
                lineValue = initValue();
            }
            else if (strcmp(first->atom, "while") == 0) {
                expr iftrue = initExpr(SEXPR);
                appendExpr(&iftrue, *getExpr(exprI, 2));
                while (execute(getExpr(exprI, 1), *currentStack).Gen.Integer != 0 || execute(getExpr(exprI, 1), *currentStack).Gen.Float != 0.0) {
                    lineValue = execExprList(&iftrue, NULL, 0, *currentStack, NULL);
                }
            }
            else if (strcmp(first->atom, "do") == 0) {
                expr toDo = initExpr(SEXPR);
                for (int j = 1; j < exprI->length; ++j) {
                    appendExpr(&toDo, *getExpr(exprI, j));
                }
                lineValue = execExprList(&toDo, NULL, 0, *currentStack, NULL);
            }
            else if (strcmp(first->atom, "library") == 0) {
                FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
                if (!stream) {
                    printf("library: Error: no such file.\n");
                    exit(-1);
                }
                char *buffer = NULL;
                size_t len = 0;
                ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
                int end = 0;
                expr libModule = fullParse(buffer, strlen(buffer), MODULE, &end);
                free(buffer);
                scopes mainScopes = initScopes();
                map *libSymbols = (map *) malloc(sizeof(map));
                execExprList(&libModule, NULL, 0, mainScopes, libSymbols);
                variable thisLibrary = initVariable();
                int libNameLength = getPrefix(getExpr(exprI, 1)->atom); // Gets everything before any file extension
                if (libNameLength == 0) {
                    libNameLength = strlen(getExpr(exprI, 1)->atom);
                }
                if (exprI->length == 3) { // Allows declaration of a library with alternate name.
                    char *libAsName = getExpr(exprI, 2)->atom;
                    thisLibrary.name = (char *) malloc((strlen(libAsName) + 1) * sizeof(char));
                    memcpy(thisLibrary.name, libAsName, strlen(libAsName));
                }
                else {
                    thisLibrary.name = (char *) malloc((libNameLength + 1) * sizeof(char));
                    thisLibrary.name[libNameLength] = '\0';
                    memcpy(thisLibrary.name, getExpr(exprI, 1)->atom, libNameLength);
                }
                printf("Library name: %s.\n", thisLibrary.name);
                thisLibrary.Var.Library = libSymbols;
                newVariable(currentStack->stack, thisLibrary);
            }
            else if (strcmp(first->atom, "include") == 0) {
                FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
                if (!stream) {
                    printf("include: Error: no such file.\n");
                    exit(-1);
                }

                char *buffer = NULL;
                size_t len = 0;
                ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
                int end = 0;
                expr included = fullParse(buffer, strlen(buffer), MODULE, &end);
                free(buffer);

                map *includedSymbols = (map *) malloc(sizeof(map));
                execExprList(&included, NULL, 0, initScopes(), includedSymbols);
                for (int j = 0; j < includedSymbols->totalBuckets; ++j) {
                    bucket thisBucket = *(includedSymbols->Buckets + j);
                    for (int k = 0; k < thisBucket.length; ++k) {
                        variable newItem = *(thisBucket.contents + k);
                        newItem.name = (char *) malloc(strlen((getExpr(exprI, 1)->atom) + 2 + strlen((thisBucket.contents + k)->name)) * sizeof(char));
                        sprintf(newItem.name, "%s", (thisBucket.contents + k)->name);
                        newVariable(currentStack->stack, newItem);
                    }
                }
            }
            else if (strcmp(first->atom, "macros") == 0) {
                FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
                if (!stream) {
                    printf("macros: Error: no such file.");
                    exit(-1);
                }
                char *buffer = NULL;
                size_t len = 0;
                ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
                macroArray *thisMacroArray = (macroArray *) malloc(sizeof(macroArray));
                int end = 0;
                parse(buffer, strlen(buffer), MODULE, &end, thisMacroArray);
                postParser(exprList, *thisMacroArray);
            }
            else {
                lineValue = execute(exprI, *currentStack);
            }
        }
    }
    if (ExportTo && exprList->label == MODULE) {
        *ExportTo = initMap();
        for (int i = 0; i < currentStack->stack->totalBuckets; ++i) {
            bucket thisBucket = *(currentStack->stack->Buckets + i);
            for (int j = 0; j < thisBucket.length; ++j) {
                newVariable(ExportTo, *(thisBucket.contents + j));
            }
        }
    }
    // Free memory and then return value of last line.
    free(first);
    free(exprI);
    free(currentStack);
    return lineValue;
}












