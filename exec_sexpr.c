#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "pointers.h"
#include "file_open.h"
#include "map.h"
#include "interpret.h"
#include "exec_sexpr.h"
#include "smart_str.h"

variable *getVariableFromLibrary(map *library, char *key)
{
    // Gets a variable from a hash table.
    variable *var_ptr = mapGet(library, key);
    if (var_ptr) {
        return var_ptr;
    }

    printf("Error: variable %s not found (in library).\n", key);
    exit(-1);
}

value execSexpr(expr *Expression, scopes currentStack)
{
    char *first = getExpr(Expression, 0)->atom;
    if (strcmp(first, "^") == 0 || strcmp(first, "|") == 0 || strcmp(first, "&") == 0 || strcmp(first, "<<") == 0 || strcmp(first, ">>") == 0) {
        return bitwiseWithCommand(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack), first);
    }
    else if (strcmp(first, "~") == 0) {
        value returnVal = initValue();
        returnVal.type = INT;
        returnVal.Gen.Integer = ~(execute(getExpr(Expression, 1), currentStack).Gen.Integer);
        return returnVal;
    }
    else if (strcmp(first, "+") == 0) {
        value x = execute(getExpr(Expression, 1), currentStack);
        for (int i = 2; i < Expression->length; ++i) {
            value y = execute(getExpr(Expression, i), currentStack);
            if (y.type == FLOAT && x.type == INT) {
                x.type = FLOAT;
                x.Gen.Float = (double) x.Gen.Integer;
            }
            if (x.type == FLOAT) {
                if (y.type == INT) {
                    x.Gen.Float += (double) y.Gen.Integer;
                }
                else {
                    x.Gen.Float += y.Gen.Float;
                }
            }
            else {
                x.Gen.Integer += y.Gen.Integer;
            }
        }
        return x;
    }
    else if (strcmp(first, "-") == 0) {
        value x = execute(getExpr(Expression, 1), currentStack);
        for (int i = 2; i < Expression->length; ++i) {
            value y = execute(getExpr(Expression, i), currentStack);
            if (y.type == FLOAT && x.type == INT) {
                x.type = FLOAT;
                x.Gen.Float = (double) x.Gen.Integer;
            }
            if (x.type == FLOAT) {
                if (y.type == INT) {
                    x.Gen.Float -= (double) y.Gen.Integer;
                }
                else {
                    x.Gen.Float -= y.Gen.Float;
                }
            }
            else {
                x.Gen.Integer -= y.Gen.Integer;
            }
        }
        return x;
    }
    else if (strcmp(first, "*") == 0) {
        value x = execute(getExpr(Expression, 1), currentStack);
        for (int i = 2; i < Expression->length; ++i) {
            value y = execute(getExpr(Expression, i), currentStack);
            if (y.type == FLOAT && x.type == INT) {
                x.type = FLOAT;
                x.Gen.Float = (double) x.Gen.Integer;
            }
            if (x.type == FLOAT) {
                if (y.type == INT) {
                    x.Gen.Float *= (double) y.Gen.Integer;
                }
                else {
                    x.Gen.Float *= y.Gen.Float;
                }
            }
            else {
                x.Gen.Integer *= y.Gen.Integer;
            }
        }
        return x;
    }
    else if (strcmp(first, "/") == 0) {   // Division always returns a float.
        value x = execute(getExpr(Expression, 1), currentStack);
        if (x.type == INT) {
            x.type = FLOAT;
            x.Gen.Float = (double) x.Gen.Integer;
        }
        for (int i = 2; i < Expression->length; ++i) {
            value y = execute(getExpr(Expression, i), currentStack);
            if (y.type == INT) {
                x.Gen.Float /= (double) y.Gen.Integer;
            }
            else {
                x.Gen.Float /= y.Gen.Float;
            }
        }
        return x;
    }
    else if (strcmp(first, "%") == 0) {
        value x = execute(getExpr(Expression, 1), currentStack);
        value y = execute(getExpr(Expression, 2), currentStack);
        if (x.type != INT || y.type != INT) {
            printf("Error: Modulus only works with integers.\n");
            exit(-1);
        }
        x.Gen.Integer %= y.Gen.Integer;
        return x;
    }
    else if (strcmp(first, "=") == 0) {
        value returnVal = initValue();
        returnVal.type = INT;
        value operand1 = execute(getExpr(Expression, 1), currentStack);
        value operand2 = execute(getExpr(Expression, 2), currentStack);
        if (operand1.type == INT && operand2.type == INT) {
            if (operand1.Gen.Integer == operand2.Gen.Integer) {
                returnVal.Gen.Integer = 1;
            }
        }
        else if (operand1.type == INT && operand2.type == FLOAT) {
            if ((double) operand1.Gen.Integer == operand2.Gen.Float) {
                returnVal.Gen.Integer = 1;
            }
        }
        else if (operand1.type == FLOAT && (double) operand2.type == INT) {
            if (operand1.Gen.Float == operand2.Gen.Integer) {
                returnVal.Gen.Integer = 1;
            }
        }
        else if (operand1.type == FLOAT && operand2.type == FLOAT) {
            if (operand1.Gen.Float == operand2.Gen.Float) {
                returnVal.Gen.Integer = 1;
            }
        }
        if (operand1.Gen.Integer == operand2.Gen.Integer && operand1.Gen.Float == operand2.Gen.Float) {
            if (operand1.type == STRING && operand2.type == STRING && strcmp(operand1.Gen.String, operand2.Gen.String) == 0) {
                returnVal.Gen.Integer = 1;
            }
            if (operand1.type != STRING && operand2.type != STRING) {
                returnVal.Gen.Integer = 1;
            }
        }
        return returnVal;
    }
    else if (strcmp(first, "not") == 0) {
        value returnVal = initValue();
        returnVal.type = INT;
        if (execute(getExpr(Expression, 1), currentStack).Gen.Integer == 0) {
            returnVal.Gen.Integer = 1;
        }
        else {
            returnVal.Gen.Integer = 0;
        }
        return returnVal;
    }
    else if (strcmp(first, ">") == 0) {
        value returnVal = initValue();
        returnVal.type = INT;
        value operand1 = execute(getExpr(Expression, 1), currentStack);
        value operand2 = execute(getExpr(Expression, 2), currentStack);
        bool isGreaterThan = false;
        if (operand1.type == INT && operand2.type == INT) {
            if (operand1.Gen.Integer > operand2.Gen.Integer) {
                isGreaterThan = true;
            }
        }
        else if (operand1.type == INT && operand2.type == FLOAT) {
            if (((double) operand1.Gen.Integer) > operand2.Gen.Float) {
                isGreaterThan = true;
            }
        }
        else if (operand1.type == FLOAT && operand2.type == INT) {
            if (operand1.Gen.Float > ((double) operand2.Gen.Integer)) {
                isGreaterThan = true;
            }
        }
        else if (operand1.type == FLOAT && operand2.type == FLOAT) {
            if (operand1.Gen.Float > operand2.Gen.Float) {
                isGreaterThan = true;
            }
        }
        if (isGreaterThan) {
            returnVal.Gen.Integer = 1;
        }
        else {
            returnVal.Gen.Integer = 0;
        }
        return returnVal;
    }
    else if (strcmp(first, "get") == 0) {
        value thisList = execute(getExpr(Expression, 1), currentStack);
        value n = execute(getExpr(Expression, 2), currentStack);
        if (thisList.type == ARRAY) {
            if (n.Gen.Integer > thisList.arrayLength - 1) {
                printf("Error: Index out of range.\n");
                exit(-1);
            }
            return *(thisList.Array + execute(getExpr(Expression, 2), currentStack).Gen.Integer);
        }
        else if (thisList.type == STRING) {
            if (n.Gen.Integer > strlen(thisList.Gen.String) - 1) {
                printf("Error: Index out of range.");
                exit(-1);
            }

            value returnVal = initValue();
            returnVal.type = STRING;
            returnVal.Gen.String = (char *) malloc(2 * sizeof(char));
            sprintf(returnVal.Gen.String, "%c", *(thisList.Gen.String + n.Gen.Integer));
            return returnVal;
        }
    }
    else if (strcmp(first, "append") == 0) {
        value returnVal = initValue();
        returnVal.type = ARRAY;
        value source = getVariable(currentStack, getExpr(Expression, 1)->atom)->Var.Value;
        returnVal.arrayLength = source.arrayLength + 1;
        returnVal.Array = (value *) malloc(returnVal.arrayLength * sizeof(value));
        memcpy(returnVal.Array, source.Array, source.arrayLength * sizeof(value));
        returnVal.Array[returnVal.arrayLength - 1] = execute(getExpr(Expression, 2), currentStack);
        return returnVal;
    }
    else if (strcmp(first, "slice") == 0) {
        int from = execute(getExpr(Expression, 2), currentStack).Gen.Integer;
        int to = execute(getExpr(Expression, 3), currentStack).Gen.Integer;

        value returnVal = initValue();
        value source = getVariable(currentStack, getExpr(Expression, 1)->atom)->Var.Value;
        if (source.type == ARRAY) {
            returnVal.type = ARRAY;
            returnVal.arrayLength = to - from;
            returnVal.Array = (value *) malloc(returnVal.arrayLength * sizeof(value));
            memcpy(returnVal.Array, source.Array + from, to * sizeof(value));
        }
        else if (source.type == STRING) {
            returnVal.type = STRING;
            returnVal.Gen.String = (char *) malloc((to - from + 1) * sizeof(char));
            memcpy(returnVal.Gen.String, source.Gen.String + from, to - from);
            returnVal.Gen.String[to - from] = '\0';
        }

        return returnVal;
    }
    else if (strcmp(first, "len") == 0) {
        value thisArray = execute(getExpr(Expression, 1), currentStack);
        value returnVal = initValue();
        returnVal.type = INT;
        if (thisArray.type == ARRAY) {
            returnVal.Gen.Integer = thisArray.arrayLength;
        }
        else if (thisArray.type == STRING) {
            returnVal.Gen.Integer = strlen(thisArray.Gen.String);
        }
        else {
            printf("Error: argument to 'len' must be array or string.");
            exit(-1);
        }

        return returnVal;
    }
    else if (strcmp(first, "print") == 0) {
        for (int i = 1; i < Expression->length; ++i) {
            value thisArg = execute(getExpr(Expression, i), currentStack);
            if (thisArg.type == INT) {
                printf("%d", thisArg.Gen.Integer);
            }
            else if (thisArg.type == FLOAT) {
                printf("%f", thisArg.Gen.Float);
            }
            else if (thisArg.type == STRING) {
                printf("%s", thisArg.Gen.String);
            }
            else if (thisArg.type == ADDRESS) {
                printf("%p", (void *) thisArg.Gen.LongInt);
            }
        }
        return initValue();
    }
    else if (strcmp(first, "sprint") == 0) {
        value newString = initValue();
        newString.type = STRING;
        for (int i = 1; i < Expression->length; ++i) {
            value thisArg = execute(getExpr(Expression, i), currentStack);
            value thisStr = initValue();
            thisStr.type = STRING;
            thisStr.Gen.String = (char *) malloc(41 * sizeof(char)); // The maximum length of a double in C (in decimal digits).
            if (thisArg.type == INT) {
                sprintf(thisStr.Gen.String, "%d", thisArg.Gen.Integer);
            }
            else if (thisArg.type == FLOAT) {
                sprintf(thisStr.Gen.String, "%f", thisArg.Gen.Float);
            }
            else if (thisArg.type == STRING) {
                thisStr.Gen.String = (char *) malloc((strlen(thisArg.Gen.String) + 1) * sizeof(char));
                strcpy(thisStr.Gen.String, thisArg.Gen.String);
            }
            else if (thisArg.type == ADDRESS) {
                sprintf(thisStr.Gen.String, "%p", (void *) thisArg.Gen.LongInt);
            }

            char *oldContents = newString.Gen.String;
            newString.Gen.String = (char *) malloc((strlen(newString.Gen.String) + strlen(thisStr.Gen.String) + 1) * sizeof(char));
            strcpy(newString.Gen.String, oldContents);
            free(oldContents);
            strcat(newString.Gen.String, thisStr.Gen.String);
            free(thisStr.Gen.String);
        }
        return newString;
    }
    else if (strcmp(first, "input") == 0) {
        value returnVal = initValue();
        returnVal.type = STRING;
        value prompt = execute(getExpr(Expression, 1), currentStack);
        if (prompt.type != STRING) {
            printf("Error: Prompt must be string.\n");
            exit(-1);
        }

        printf("%s", prompt.Gen.String);
        char c = getchar();
        smart_str_t *tmpStr = emptyStr();
        for (int i = 0; c != '\n'; ++i) {
            str_putc(tmpStr, c);
            c = getchar();
        }

        returnVal.Gen.String = getContentsCpy(tmpStr);
        return returnVal;
    }
    else if (strcmp(first, "Int") == 0 || strcmp(first, "String") == 0 || strcmp(first, "Float") == 0) {
        return typecast(execute(getExpr(Expression, 1), currentStack), first);
    }
    else if (strcmp(first, "ord") == 0) {
        value arg = execute(getExpr(Expression, 1), currentStack);
        if (arg.type != STRING) {
            printf("Error! ord function can only take a string.\n");
        }

        char *argStr = arg.Gen.String;
        if (strlen(argStr) != 1) {
            printf("Error! ord requires a one-character argument. Got: %s\n", argStr);
        }

        value returnVal = initValue();
        returnVal.type = INT;
        returnVal.Gen.Integer = (int) argStr[0];
        return returnVal;
    }
    else if (strcmp(first, "char") == 0) {
        char character = execute(getExpr(Expression, 1), currentStack).Gen.Integer;
        value returnVal = initValue();
        returnVal.type = STRING;
        returnVal.Gen.String = (char *) malloc(2 * sizeof(char));
        sprintf(returnVal.Gen.String, "%c", character);
        return returnVal;
    }
    else if (strcmp(first, "exit") == 0) {
        exit(execute(getExpr(Expression, 1), currentStack).Gen.Integer);
    }
    else if (strcmp(first, "Int*") == 0 || strcmp(first, "Float*") == 0 || strcmp(first, "String*") == 0 || strcmp(first, "Array*") == 0) {
        value address = execute(getExpr(Expression, 1), currentStack);
        return retrieveWithCommand(address, first);
    }
    else if (strcmp(first, "address") == 0) {
        variable *thisVarPtr = getVariable(currentStack, getExpr(Expression, 1)->atom);
        return getAddressOf(thisVarPtr);
    }
    else if (strcmp(first, "alter") == 0) {
        reassignAddress(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
        return initValue();
    }
    if (strcmp(first, "fopenb") == 0) {
        return fileOpenBinary(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    }
    else if (strcmp(first, "fopent") == 0) {
        return fileOpenText(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    }
    else if (strcmp(first, "write") == 0) {
        writeToFile(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
        return initValue();
    }
    else if (strcmp(first, "writeByte") == 0) {
        writeToFileInt(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
        return initValue();
    }
    else if (strcmp(first, "readn") == 0) {
        return readFromFile(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    }
    else if (strcmp(first, "readDelim") == 0) {
        return readFromFileDelim(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    }
    else if (strcmp(first, "fclose") == 0) {
        fclose(execute(getExpr(Expression, 1), currentStack).Gen.filestream);
        return initValue();
    }
    else {
        char *fullname = getExpr(Expression, 0)->atom;
        variable thisFunction;
        scopes funcScopes = initScopes();
        int preLength = getPrefix(fullname);
        if (preLength > 0) {
            char *prefix = malloc(preLength * sizeof(char));
            memcpy(prefix, fullname, preLength);
            char *functionName = malloc((strlen(fullname) - preLength) * sizeof(char));
            memcpy(functionName, fullname + preLength + 1, strlen(fullname) - preLength - 1);
            thisFunction = *getVariableFromLibrary(getVariable(currentStack, prefix)->Var.Library, functionName);
            funcScopes = plusScope(currentStack, *(getVariable(currentStack, prefix)->Var.Library));
            free(prefix);
            free(functionName);
        }
        else {
            thisFunction = *getVariable(currentStack, fullname);
            funcScopes = currentStack;
        }

        if (thisFunction.Var.Function.argumentNumber != Expression->length - 1) {
            printf("Error: Incorrect number of arguments for %s.\n", thisFunction.name);
            exit(-1);
        }

        variable *arguments = (variable *) malloc(thisFunction.Var.Function.argumentNumber * sizeof(variable));
        for (int j = 0; j < Expression->length - 1; ++j) {
            arguments[j].name = *(thisFunction.Var.Function.argumentNames + j);
            arguments[j].Var.Value = execute(getExpr(Expression, j + 1), currentStack);
        }

        value returnVal = execExprList(&(thisFunction.Var.Function.body), arguments, thisFunction.Var.Function.argumentNumber, funcScopes, NULL);
        free(arguments);
        return returnVal;
    }

    return initValue();
}