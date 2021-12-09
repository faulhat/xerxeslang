#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "file_open.h"
// file_open.h includes pointers.h, which already includes macros.h, which already includes proto_2.h

// Data structures and related information

// x2 and x3 tables in Rijndael's finite field
// for use in hashing

const uint8_t Gm2[256] = {
  0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,
  0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,
  0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
  0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,
  0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,
  0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
  0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
  0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe,
  0x1b, 0x19, 0x1f, 0x1d, 0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,
  0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d, 0x23, 0x21, 0x27, 0x25,
  0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55, 0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45,
  0x7b, 0x79, 0x7f, 0x7d, 0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65,
  0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d, 0x83, 0x81, 0x87, 0x85,
  0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5, 0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5,
  0xdb, 0xd9, 0xdf, 0xdd, 0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5,
  0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed, 0xe3, 0xe1, 0xe7, 0xe5
};

const uint8_t Gm3[256] = {
  0x00, 0x03, 0x06, 0x05, 0x0c, 0x0f, 0x0a, 0x09, 0x18, 0x1b, 0x1e, 0x1d, 0x14, 0x17, 0x12, 0x11,
  0x30, 0x33, 0x36, 0x35, 0x3c, 0x3f, 0x3a, 0x39, 0x28, 0x2b, 0x2e, 0x2d, 0x24, 0x27, 0x22, 0x21,
  0x60, 0x63, 0x66, 0x65, 0x6c, 0x6f, 0x6a, 0x69, 0x78, 0x7b, 0x7e, 0x7d, 0x74, 0x77, 0x72, 0x71,
  0x50, 0x53, 0x56, 0x55, 0x5c, 0x5f, 0x5a, 0x59, 0x48, 0x4b, 0x4e, 0x4d, 0x44, 0x47, 0x42, 0x41,
  0xc0, 0xc3, 0xc6, 0xc5, 0xcc, 0xcf, 0xca, 0xc9, 0xd8, 0xdb, 0xde, 0xdd, 0xd4, 0xd7, 0xd2, 0xd1,
  0xf0, 0xf3, 0xf6, 0xf5, 0xfc, 0xff, 0xfa, 0xf9, 0xe8, 0xeb, 0xee, 0xed, 0xe4, 0xe7, 0xe2, 0xe1,
  0xa0, 0xa3, 0xa6, 0xa5, 0xac, 0xaf, 0xaa, 0xa9, 0xb8, 0xbb, 0xbe, 0xbd, 0xb4, 0xb7, 0xb2, 0xb1,
  0x90, 0x93, 0x96, 0x95, 0x9c, 0x9f, 0x9a, 0x99, 0x88, 0x8b, 0x8e, 0x8d, 0x84, 0x87, 0x82, 0x81,
  0x9b, 0x98, 0x9d, 0x9e, 0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8f, 0x8c, 0x89, 0x8a,
  0xab, 0xa8, 0xad, 0xae, 0xa7, 0xa4, 0xa1, 0xa2, 0xb3, 0xb0, 0xb5, 0xb6, 0xbf, 0xbc, 0xb9, 0xba,
  0xfb, 0xf8, 0xfd, 0xfe, 0xf7, 0xf4, 0xf1, 0xf2, 0xe3, 0xe0, 0xe5, 0xe6, 0xef, 0xec, 0xe9, 0xea,
  0xcb, 0xc8, 0xcd, 0xce, 0xc7, 0xc4, 0xc1, 0xc2, 0xd3, 0xd0, 0xd5, 0xd6, 0xdf, 0xdc, 0xd9, 0xda,
  0x5b, 0x58, 0x5d, 0x5e, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46, 0x4f, 0x4c, 0x49, 0x4a,
  0x6b, 0x68, 0x6d, 0x6e, 0x67, 0x64, 0x61, 0x62, 0x73, 0x70, 0x75, 0x76, 0x7f, 0x7c, 0x79, 0x7a,
  0x3b, 0x38, 0x3d, 0x3e, 0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2f, 0x2c, 0x29, 0x2a,
  0x0b, 0x08, 0x0d, 0x0e, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16, 0x1f, 0x1c, 0x19, 0x1a
};

expr initExpr(int label) {
  // Initializes an expression with a given label.
  expr Expression;
  Expression.label = label;
  Expression.length = 0;
  Expression.atom = NULL;
  Expression.sub = NULL;
  return Expression;
}

// Parser

void printExpr(expr* Expression) {
  // Prints the label of an expression and a summary of its contents.
  if (!Expression) {
    printf("Expression is null.\n");
  }
  printf("Label: %d\n", Expression->label);
  if (Expression->label == ATOM || Expression->label == INT || Expression->label == FLOAT || Expression->label == STRING) {
    printf("Atom contents: %s\n", Expression->atom);
  } else {
    printf("Number of nested expressions: %d\n", Expression->length);
  }
}

void appendExpr(expr* Expression, expr element) {
  // Adds an expression to a list.
  Expression->atom = NULL;
  ++(Expression->length);
  Expression->sub = (expr *) realloc(Expression->sub, Expression->length * sizeof(expr));
  *(Expression->sub + Expression->length - 1) = element;
}

expr assignAtom(char* contents, int length) {
  // Creates an atom containing the given name or literal.
  expr Expression = initExpr(ATOM);
  Expression.length = 0;
  Expression.sub = NULL;
  bool isInt = true;
  bool isFloat = false;
  Expression.atom = (char *) calloc(length + 1, sizeof(char));
  for (int i = 0; i < length; ++i) {
    *(Expression.atom + i) = *(contents + i);
    if (('0' > *(contents + i) || *(contents + i) > '9') && !(*(contents + i) == '-' && i == 0)) {
      if (isInt && *(contents + i) == '.') {
        isFloat = true;
      } else {
        isFloat = false;
      }
      isInt = false;
    }
  }
  if (Expression.label != STRING) {
    if (isInt) {
      Expression.label = INT;
    } else if (isFloat) {
      Expression.label = FLOAT;
    }
  }
  return Expression;
}

expr *getExpr(expr* Expression, int n) {
  // Retrieves element n from the list in the expr structure.
  if (n > Expression->length) {
    printf("Parser Error: Out of range.\n");
    exit(-1);
  }
  return (Expression->sub + n);
}

expr parse(char *code, int length, int label, int *cursor, macroArray *programMacros) {
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
    } else if (label == SEXPR || label == MODULE || label == ARRAY || label == MACRO) {
      if (token == ')' && label == SEXPR) { // Handles s-expressions
        break;
      } else if (token == ']' && label == ARRAY) { // Handles arrays
        break;
      } else if (token == '}' && label == MACRO) { // Handles macros
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
      } else if (token == '(') {
        ++(*cursor);
        appendExpr(&Expression, parse(code, length, SEXPR, cursor, programMacros)); // Reads s-expressions
      } else if (token == '[') {
        ++(*cursor);
        appendExpr(&Expression, parse(code, length, ARRAY, cursor, programMacros)); // Reads arrays
      } else if (token == '{') {
        ++(*cursor);
        parse(code, length, MACRO, cursor, programMacros);
      } else if (token == '"') {
        ++(*cursor);
        appendExpr(&Expression, parse(code, length, STRING, cursor, programMacros)); // Reads strings
      } else if (token == ';') {
        while (token != '\n' && *cursor < length) {
          ++(*cursor);
          token = *(code + *cursor);
        }
      } else if (token != ' ' && token != '\n' && token != '\t') {
        appendExpr(&Expression, parse(code, length, ATOM, cursor, programMacros)); // Reads other atoms
      }
    } else if (label == STRING) {
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

expr fullParse(char* code, int length, int label, int *cursor) {
  macroArray programMacros;
  programMacros.macroNumber = 0;
  programMacros.Array = NULL;
  expr parseTree = parse(code, length, label, cursor, &programMacros);
  postParser(&parseTree, programMacros);
  return parseTree;
}

// Interpreter (Prototype 2, can handle multiple types)

// These "init" functions set all values in a struct to zero
value initValue() {
  value newVal;
  newVal.type = 0;
  newVal.Gen.LongInt = 0;
  newVal.arrayLength = 0;
  newVal.Array = NULL;
  return newVal;
}

function initFunction() {
  function newFunc;
  newFunc.body = initExpr(FUNCTION);
  newFunc.argumentNames = NULL;
  newFunc.argumentNumber = 0;
  return newFunc;
}

variable initVariable() {
  variable newVar;
  newVar.name = NULL;
  newVar.hash = 0;
  newVar.Var.Value = initValue();
  return newVar;
}

map initMap(int hashLen) {
  map newMap;
  newMap.hashLength = hashLen;
  newMap.totalBuckets = 1 << hashLen;
  newMap.usedBuckets = 0;
  newMap.Buckets = (bucket *) calloc(newMap.totalBuckets, sizeof(bucket));
  for (int i = 0; i < newMap.totalBuckets; ++i) {
    (newMap.Buckets + i)->length = 0;
    (newMap.Buckets + i)->contents = NULL;
  }
  return newMap;
}

scopes initScopes() {
  scopes newStack;
  newStack.depth = 0;
  newStack.stack = NULL;
  return newStack;
}

uint32_t hashName(char *name) {
  // Sponge construction with a state size of 32 and a rate of 8.
  // Function is matrix multiplication in Rijndael's Finite Field.
  uint32_t sponge = 0;
  for (int i = 0; i < strlen(name); i += 2) {
    sponge ^= (uint32_t) *(name + i);
    if (strlen(name) == i + 1) {
      sponge ^= ((uint32_t) *(name + i + 1)) << 8;
    }
    int temp = 0;
    for (int j = 0; j < 4; ++j) {
      temp ^= Gm2[(sponge >> ((j * 8) % 32)) & 0xff];
      temp ^= Gm3[(sponge >> ((j * 8 + 8) % 32)) & 0xff];
      temp ^= (sponge >> ((j * 8 + 16) % 32)) & 0xff;
      temp ^= (sponge >> ((j * 8 + 24) % 32)) & 0xff;
    }
    sponge = temp;
  }
  return sponge;
}

bucket addToBucket(bucket *target, variable newContent) {
  // Adds a variable to a bucket and returns the result.
  // New contents with the same name as old contents overwrites the old contents.
  bucket newBucket;
  for (int i = 0; i < target->length; ++i) {
    if (strcmp((target->contents + i)->name, newContent.name) == 0) {
      newBucket.contents = (variable *) calloc(target->length, sizeof(variable));
      memcpy(newBucket.contents, target->contents, target->length * sizeof(variable));
      *(newBucket.contents + i) = newContent;
      return newBucket;
    }
  }
  newBucket.length = target->length + 1;
  newBucket.contents = (variable *) calloc(newBucket.length, sizeof(variable));
  memcpy(newBucket.contents, target->contents, target->length * sizeof(variable));
  *(newBucket.contents + newBucket.length - 1) = newContent;
  return newBucket;
}

void reorganize(map *target, int newHashLength) {
  // Reorganizes a hash table into one of a different hash length.
  map temp = initMap(newHashLength);
  uint32_t mask = 0;
  mask = ~mask >> (32 - newHashLength);
  for (int i = 0; i < target->totalBuckets; ++i) {
    bucket thisBucket = *(target->Buckets + i);
    for (int j = 0; j < thisBucket.length; ++j) {
      variable content = *(thisBucket.contents + j);
      *(temp.Buckets + (content.hash & mask)) = addToBucket(temp.Buckets + (content.hash & mask), content);
    }
  }
  *target = temp;
}

void newVariable(map *destination, variable item) {
  // Adds a new variable to a hash table, expanding the table if necessary.
  if (destination->usedBuckets > 7 * destination->totalBuckets / 8) {
    reorganize(destination, destination->hashLength + 1);
  }
  uint32_t mask = 0;
  mask = ~mask >> (32 - destination->hashLength);
  item.hash = hashName(item.name);
  *(destination->Buckets + (item.hash & mask)) = addToBucket(destination->Buckets + (item.hash & mask), item);  
}

scopes plusScope(scopes currentStack, map newScope) {
  // Adds a new scope to the stack of scopes.
  scopes newStack;
  newStack.depth = currentStack.depth + 1;
  newStack.stack = (map *) calloc(newStack.depth, sizeof(map));
  *(newStack.stack) = newScope;
  for (int i = 1; i < newStack.depth; ++i) {
    memcpy(newStack.stack + i, currentStack.stack + i - 1, sizeof(map));
  }
  return newStack;
}

variable *getVariable(scopes currentStack, char *key) {
  // Gets a variable from a stack of maps.
  for (int i = 0; i < currentStack.depth; ++i) {
    map thisScope = *(currentStack.stack + i);
    uint32_t mask = 0;
    mask = ~mask >> (32 - thisScope.hashLength);
    int keyHash = hashName(key) & mask;
    bucket keyHashBucket = *(thisScope.Buckets + keyHash);
    for (int j = 0; j < keyHashBucket.length; ++j) {
      if (strcmp((keyHashBucket.contents + j)->name, key) == 0) {
        return keyHashBucket.contents + j;
      }
    }
  }
  printf("Name Error: %s not found.\n", key);
  exit(-1);
}

value typecast(value source, char *cast) {
  // Casts a "value" from one type to another.
  value returnVal = initValue();
  if (source.type == STRING) {
    if (strcmp(cast, "String") == 0) {
      return source;
    }
    if (strlen(source.Gen.String) != 1) {
      printf("Type Error: cannot coerce string to type %s\n", cast);
      exit(-1);
    }
    if (strcmp(cast, "Int") == 0) {
      returnVal.type = INT;
      returnVal.Gen.Integer = (int) *(source.Gen.String);
      return returnVal;
    } else if (strcmp(cast, "Float") == 0) {
      returnVal.type = FLOAT;
      returnVal.Gen.Float = (double) ((int) *(source.Gen.String));
      return returnVal;
    }
  } else if (source.type == INT) {
    if (strcmp(cast, "Int") == 0) {
      return source;
    } else if (strcmp(cast, "Float") == 0) {
      returnVal.type = FLOAT;
      returnVal.Gen.Float = (double) source.Gen.Integer;
      return returnVal;
    } else if (strcmp(cast, "String") == 0) {
      returnVal.type = STRING;
      char *buffer = (char *) calloc(20, sizeof(char));
      sprintf(buffer, "%d", source.Gen.Integer);
      returnVal.Gen.String = (char *) calloc(strlen(buffer) + 1, sizeof(char));
      memcpy(returnVal.Gen.String, buffer, strlen(buffer));
      free(buffer);
      return returnVal;
    }
  } else if (source.type == FLOAT) {
    if (strcmp(cast, "Float") == 0) {
      return source;
    } else if (strcmp(cast, "Int") == 0) {
      returnVal.type = INT;
      returnVal.Gen.Integer = (int) source.Gen.Float;
      return returnVal;
    } else if (strcmp(cast, "String") == 0) {
      returnVal.type = STRING;
      char *buffer = (char *) calloc(20, sizeof(char));
      sprintf(buffer, "%f", source.Gen.Float);
      returnVal.Gen.String = (char *) calloc(strlen(buffer) + 1, sizeof(char));
      memcpy(returnVal.Gen.String, buffer, strlen(buffer));
      free(buffer);
      return returnVal;
    }
  }
  printf("Error: Invalid cast.\n");
  exit(-1);
}

int getPrefix(char *source) {
  // Finds everything in a name up to a dot.
  // Returns the number of characters before that dot.
  for (int i = 0; i < strlen(source); ++i) {
    if (*(source + i) == '.') {
      return i;
    }
  }
  return 0; // Returns 0 if no dot is found.
}

variable *getVariableFromLibrary(map *library, char *key) {
  // Gets a variable straight from a hash table.
  uint32_t mask = 0;
  mask = ~mask >> (32 - library->hashLength);
  int keyHash = hashName(key) & mask;
  bucket keyHashBucket = *(library->Buckets + keyHash);
  for (int j = 0; j < keyHashBucket.length; ++j) {
    if (strcmp((keyHashBucket.contents + j)->name, key) == 0) {
      return keyHashBucket.contents + j;
    }
  }
  printf("Error: variable %s not found (in library).\n", key);
  exit(-1);
}

value bitwiseWithCommand(value valueOperand1, value valueOperand2, char *command) {
  int operand1 = valueOperand1.Gen.Integer;
  int operand2 = valueOperand2.Gen.Integer;
  value result = initValue();
  result.type = INT;
  if (strcmp(command, "^") == 0) {
    result.Gen.Integer = (operand1 ^ operand2);
  } else if (strcmp(command, "|") == 0) {
    result.Gen.Integer = (operand1 | operand2);
  } else if (strcmp(command, "&") == 0) {
    result.Gen.Integer = (operand1 & operand2);
  } else if (strcmp(command, ">>") == 0) {
    result.Gen.Integer = (operand1 >> operand2);
  } else if (strcmp(command, "<<") == 0) {
    result.Gen.Integer = (operand1 << operand2);
  } else {
    printf("Error: Invalid bitwise operation.");
    exit(-1);
  }
  return result;
}

// Forward declaration of execExprList for use in execute.
value execExprList(expr *exprList, variable *arguments, int numberOfArguments, scopes allHigherScopes, map *ExportTo);

value execute(expr *Expression, scopes currentStack) {
  // Executes single s-expressions and atoms.
  if (Expression->label == ATOM) { // Default ATOM label indicates a word
    return getVariable(currentStack, Expression->atom)->Var.Value;
  } else if (Expression->label == INT) { // Calculates the value of an int literal.
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
  } else if (Expression->label == FLOAT) { // Calculates the value of a float literal.
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
  } else if (Expression->label == STRING) {
    value returnVal = initValue();
    returnVal.type = STRING;
    char *temp = Expression->atom;
    returnVal.Gen.String = (char *) calloc(strlen(temp) + 1, sizeof(char));
    int backslashes = 0;
    for (int i = 0; i < strlen(temp); ++i) {
      char thisCharacter = *(temp + i);
      if (thisCharacter == '\\') {
        if (*(temp + i + 1) == '"') {
          *(returnVal.Gen.String + i - backslashes) = '"';
        } else if (*(temp + i + 1) == 'n') {
          *(returnVal.Gen.String + i - backslashes) = '\n';
        } else if (*(temp + i + 1) == 't') {
          *(returnVal.Gen.String + i - backslashes) = '\t';
        } else if (*(temp + i + 1) == '\\') {
          *(returnVal.Gen.String + i - backslashes) = '\\';
        } else {
          printf("Error: Unrecognized escape sequence.\n");
          exit(-1);
        }
        ++backslashes;
        ++i;
      } else {
        *(returnVal.Gen.String + i - backslashes) = thisCharacter;
      }
    }
    return returnVal;
  } else if (Expression->label == ARRAY) {
    value returnVal = initValue();
    returnVal.type = ARRAY;
    returnVal.Array = (value *) calloc(Expression->length, sizeof(value));
    for (int i = 0; i < Expression->length; ++i) {
      *(returnVal.Array + i) = execute(getExpr(Expression, i), currentStack);
      ++(returnVal.arrayLength);
    }
    return returnVal;
  } else if (Expression->label == SEXPR) {
    char *first = getExpr(Expression, 0)->atom;
    if (strcmp(first, "^") == 0 || strcmp(first, "|") == 0 || strcmp(first, "&") == 0 || strcmp(first, "<<") == 0 || strcmp(first, ">>") == 0) {
      return bitwiseWithCommand(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack), first);
    } else if (strcmp(first, "~") == 0) {
      value returnVal = initValue();
      returnVal.type = INT;
      returnVal.Gen.Integer = ~(execute(getExpr(Expression, 1), currentStack).Gen.Integer);
      return returnVal;
    } else if (strcmp(first, "+") == 0) {
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
          } else {
            x.Gen.Float += y.Gen.Float;
          }
        } else {
          x.Gen.Integer += y.Gen.Integer;
        }
      }
      return x;
    } else if (strcmp(first, "-") == 0) {
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
          } else {
            x.Gen.Float -= y.Gen.Float;
          }
        } else {
          x.Gen.Integer -= y.Gen.Integer;
        }
      }
      return x;
    } else if (strcmp(first, "*") == 0) {
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
          } else {
            x.Gen.Float *= y.Gen.Float;
          }
        } else {
          x.Gen.Integer *= y.Gen.Integer;
        }
      }
      return x;
    } else if (strcmp(first, "/") == 0) { // Division always returns a float.
      value x = execute(getExpr(Expression, 1), currentStack);
      if (x.type == INT) {
        x.type = FLOAT;
        x.Gen.Float = (double) x.Gen.Integer;
      }
      for (int i = 2; i < Expression->length; ++i) {
        value y = execute(getExpr(Expression, i), currentStack);
        if (y.type == INT) {
          x.Gen.Float /= (double) y.Gen.Integer;
        } else {
          x.Gen.Float /= y.Gen.Float;
        }
      }
      return x;
    } else if (strcmp(first, "%") == 0) {
      value x = execute(getExpr(Expression, 1), currentStack);
      value y = execute(getExpr(Expression, 2), currentStack);
      if (x.type != INT || y.type != INT) {
        printf("Error: Modulus only works with integers.\n");
        exit(-1);
      }
      x.Gen.Integer %= y.Gen.Integer;
      return x;
    } else if (strcmp(first, "=") == 0) {
      value returnVal = initValue();
      returnVal.type = INT;
      value operand1 = execute(getExpr(Expression, 1), currentStack);
      value operand2 = execute(getExpr(Expression, 2), currentStack);
      if (operand1.type == INT && operand2.type == INT) {
        if (operand1.Gen.Integer == operand2.Gen.Integer) {
          returnVal.Gen.Integer = 1;
        }
      } else if (operand1.type == INT && operand2.type == FLOAT) {
        if ((double) operand1.Gen.Integer == operand2.Gen.Float) {
          returnVal.Gen.Integer = 1;
        }
      } else if (operand1.type == FLOAT && (double) operand2.type == INT) {
        if (operand1.Gen.Float == operand2.Gen.Integer) {
          returnVal.Gen.Integer = 1;
        }
      } else if (operand1.type == FLOAT && operand2.type == FLOAT) {
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
    } else if (strcmp(first, "not") == 0) {
      value returnVal = initValue();
      returnVal.type = INT;
      if (execute(getExpr(Expression, 1), currentStack).Gen.Integer == 0) {
        returnVal.Gen.Integer = 1;
      } else {
        returnVal.Gen.Integer = 0;
      }
      return returnVal;
    } else if (strcmp(first, ">") == 0) {
      value returnVal = initValue();
      returnVal.type = INT;
      value operand1 = execute(getExpr(Expression, 1), currentStack);
      value operand2 = execute(getExpr(Expression, 2), currentStack);
      bool isGreaterThan = false;
      if (operand1.type == INT && operand2.type == INT) {
        if (operand1.Gen.Integer > operand2.Gen.Integer) {
          isGreaterThan = true;
        }
      } else if (operand1.type == INT && operand2.type == FLOAT) {
        if (((double) operand1.Gen.Integer) > operand2.Gen.Float) {
          isGreaterThan = true;
        }
      } else if (operand1.type == FLOAT && operand2.type == INT) {
        if (operand1.Gen.Float > ((double) operand2.Gen.Integer)) {
          isGreaterThan = true;
        }
      } else if (operand1.type == FLOAT && operand2.type == FLOAT) {
        if (operand1.Gen.Float > operand2.Gen.Float) {
          isGreaterThan = true;
        }
      }
      if (isGreaterThan) {
        returnVal.Gen.Integer = 1;
      } else {
        returnVal.Gen.Integer = 0;
      }
      return returnVal;
    } else if (strcmp(first, "get") == 0) {
      value thisList = execute(getExpr(Expression, 1), currentStack);
      value n = execute(getExpr(Expression, 2), currentStack);
      if (thisList.type == ARRAY) {
        if (n.Gen.Integer > thisList.arrayLength - 1) {
          printf("Error: Index out of range.\n");
          exit(-1);
        }
        return *(thisList.Array + execute(getExpr(Expression, 2), currentStack).Gen.Integer);
      } else if (thisList.type == STRING) {
        if (n.Gen.Integer > strlen(thisList.Gen.String) - 1) {
          printf("Error: Index out of range.");
          exit(-1);
        }
        value returnVal = initValue();
        returnVal.type = STRING;
        returnVal.Gen.String = (char *) calloc(1, sizeof(char));
        sprintf(returnVal.Gen.String, "%c", *(thisList.Gen.String + n.Gen.Integer));
        return returnVal;
      }
    } else if (strcmp(first, "append") == 0) {
      value returnVal = initValue();
      returnVal.type = ARRAY;
      value source = getVariable(currentStack, getExpr(Expression, 1)->atom)->Var.Value;
      returnVal.arrayLength = source.arrayLength + 1;
      returnVal.Array = (value *) calloc(returnVal.arrayLength, sizeof(value));
      memcpy(returnVal.Array, source.Array, source.arrayLength * sizeof(value));
      *(returnVal.Array + returnVal.arrayLength - 1) = execute(getExpr(Expression, 2), currentStack);
      return returnVal;
    } else if (strcmp(first, "slice") == 0) {
      int from = execute(getExpr(Expression, 2), currentStack).Gen.Integer;
      int to = execute(getExpr(Expression, 3), currentStack).Gen.Integer;
      value returnVal = initValue();
      value source = getVariable(currentStack, getExpr(Expression, 1)->atom)->Var.Value;
      if (source.type == ARRAY) {
        returnVal.type = ARRAY;
        returnVal.arrayLength = to - from;
        returnVal.Array = (value *) calloc(returnVal.arrayLength, sizeof(value));
        memcpy(returnVal.Array, source.Array + from, to * sizeof(value));
      } else if (source.type == STRING) {
        returnVal.type = STRING;
        returnVal.Gen.String = (char *) calloc((to - from + 1), sizeof(char));
        for (int j = 0; j < to - from; ++j) {
          *(returnVal.Gen.String + j) = *(source.Gen.String + from + j);
        }
      }
      return returnVal;
    } else if (strcmp(first, "len") == 0) {
      value thisArray = execute(getExpr(Expression, 1), currentStack);
      value returnVal = initValue();
      returnVal.type = INT;
      if (thisArray.type == ARRAY) {
        returnVal.Gen.Integer = thisArray.arrayLength;
      } else if (thisArray.type == STRING) {
        returnVal.Gen.Integer = strlen(thisArray.Gen.String);
      } else {
        printf("Error: argument to 'len' must be array or string.");
        exit(-1);
      }
      return returnVal;
    } else if (strcmp(first, "print") == 0) {
      for (int i = 1; i < Expression->length; ++i) {
        value thisArg = execute(getExpr(Expression, i), currentStack);
        if (thisArg.type == INT) {
          printf("%d", thisArg.Gen.Integer);
        } else if (thisArg.type == FLOAT) {
          printf("%f", thisArg.Gen.Float);
        } else if (thisArg.type == STRING) {
          printf("%s", thisArg.Gen.String);
        } else if (thisArg.type == ADDRESS) {
          printf("%p", (void *) thisArg.Gen.LongInt);
        }
      }
      return initValue();
    } else if (strcmp(first, "sprint") == 0) {
      value newString = initValue();
      newString.type = STRING;
      newString.Gen.String = (char *) calloc(0, sizeof(char));
      for (int i = 1; i < Expression->length; ++i) {
        value thisArg = execute(getExpr(Expression, i), currentStack);
        value thisStr = initValue();
        thisStr.type = STRING;
        thisStr.Gen.String = (char *) calloc(40, sizeof(char)); // The maximum length of a double in C (in decimal digits).
        if (thisArg.type == INT) {
          sprintf(thisStr.Gen.String, "%d", thisArg.Gen.Integer);
        } else if (thisArg.type == FLOAT) {
          sprintf(thisStr.Gen.String, "%f", thisArg.Gen.Float);
        } else if (thisArg.type == STRING) {
          thisStr.Gen.String = (char *) calloc(strlen(thisArg.Gen.String) + 1, sizeof(char));
          for (int j = 0; j < strlen(thisArg.Gen.String); ++j) {
            *(thisStr.Gen.String + j) = *(thisArg.Gen.String + j);
          }
        } else if (thisArg.type == ADDRESS) {
          sprintf(thisStr.Gen.String, "%p", (void *) thisArg.Gen.LongInt);
        }
        newString.Gen.String = (char *) realloc(newString.Gen.String, (strlen(newString.Gen.String) + strlen(thisStr.Gen.String)) * sizeof(char));
        strcat(newString.Gen.String, thisStr.Gen.String);
      }
      return newString;
    } else if (strcmp(first, "input") == 0) {
      value returnVal = initValue();
      returnVal.type = STRING;
      returnVal.Gen.String = (char *) calloc(1, sizeof(char));
      value prompt = execute(getExpr(Expression, 1), currentStack);
      if (prompt.type != STRING) {
        printf("Error: Prompt must be string.\n");
        exit(-1);
      }
      printf("%s", prompt.Gen.String);
      char c = getchar();
      for (int i = 0; c != '\n'; ++i) {
        returnVal.Gen.String = (char *) realloc(returnVal.Gen.String, (i + 1) * sizeof(int));
        *(returnVal.Gen.String + i) = c;
        c = getchar();
      }
      return returnVal;
    } else if (strcmp(first, "Int") == 0 || strcmp(first, "String") == 0 || strcmp(first, "Float") == 0) {
      return typecast(execute(getExpr(Expression, 1), currentStack), first);
    } else if (strcmp(first, "char") == 0) {
      char character = execute(getExpr(Expression, 1), currentStack).Gen.Integer;
      value returnVal = initValue();
      returnVal.type = STRING;
      returnVal.Gen.String = (char *) calloc(2, sizeof(char));
      *returnVal.Gen.String = character;
      return returnVal;
    } else if (strcmp(first, "exit") == 0) {
      exit(execute(getExpr(Expression, 1), currentStack).Gen.Integer);
    } else if (strcmp(first, "Int*") == 0 || strcmp(first, "Float*") == 0 || strcmp(first, "String*") == 0 || strcmp(first, "Array*") == 0) {
      value address = execute(getExpr(Expression, 1), currentStack);
      return retrieveWithCommand(address, first);
    } else if (strcmp(first, "address") == 0) {
      variable *thisVarPtr = getVariable(currentStack, getExpr(Expression, 1)->atom);
      return getAddressOf(thisVarPtr);
    } else if (strcmp(first, "alter") == 0) {
      reassignAddress(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
      return initValue();
    } if (strcmp(first, "fopenb") == 0) {
      return fileOpenBinary(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    } else if (strcmp(first, "fopent") == 0) {
      return fileOpenText(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    } else if (strcmp(first, "write") == 0) {
      writeToFile(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
      return initValue();
    } else if (strcmp(first, "writeByte") == 0) {
      writeToFileInt(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
      return initValue();
    } else if (strcmp(first, "readn") == 0) {
      return readFromFile(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    } else if (strcmp(first, "readDelim") == 0) {
      return readFromFileDelim(execute(getExpr(Expression, 1), currentStack), execute(getExpr(Expression, 2), currentStack));
    } else if (strcmp(first, "fclose") == 0) {
      fclose(execute(getExpr(Expression, 1), currentStack).Gen.filestream);
      return initValue();
    } else {
      char *fullname = getExpr(Expression, 0)->atom;
      variable thisFunction;
      scopes funcScopes = initScopes();
      int preLength = getPrefix(fullname);
      if (preLength > 0) {
        char *prefix = calloc(preLength + 1, sizeof(char));
        memcpy(prefix, fullname, preLength);
        char *functionName = calloc(strlen(fullname) - preLength, sizeof(char));
        memcpy(functionName, fullname + preLength + 1, strlen(fullname) - preLength - 1);
        thisFunction = *getVariableFromLibrary(getVariable(currentStack, prefix)->Var.Library, functionName);
        funcScopes = plusScope(currentStack, *(getVariable(currentStack, prefix)->Var.Library));
        free(prefix);
        free(functionName);
      } else {
        thisFunction = *getVariable(currentStack, fullname);
        funcScopes = currentStack;
      }
      if (thisFunction.Var.Function.argumentNumber != Expression->length - 1) {
        printf("Error: Incorrect number of arguments for %s.\n", thisFunction.name);
        exit(-1);
      }
      variable *arguments = (variable *) calloc(thisFunction.Var.Function.argumentNumber, sizeof(variable));
      for (int j = 0; j < Expression->length - 1; ++j) {
        (arguments + j)->name = *(thisFunction.Var.Function.argumentNames + j);
        (arguments + j)->Var.Value = execute(getExpr(Expression, j + 1), currentStack);
      }
      return execExprList(&(thisFunction.Var.Function.body), arguments, thisFunction.Var.Function.argumentNumber, funcScopes, NULL);
    }
  }
  return initValue();
}


value execExprList(expr *exprList, variable *arguments, int numberOfArguments, scopes allHigherScopes, map *ExportTo) {
  // Executes lists of s-expressions and atoms, such as modules and functions.
  scopes *currentStack = (scopes *) calloc(allHigherScopes.depth + 1, sizeof(scopes));
  *currentStack = plusScope(allHigherScopes, initMap(6));
  for (int i = 0; i < numberOfArguments; ++i) {
    newVariable((currentStack->stack), *(arguments + i));
  }
  expr *exprI = (expr *) calloc(1, sizeof(expr));
  expr *first = (expr *) calloc(1, sizeof(expr));
  value lineValue = initValue();
  for (int i = 0; i < exprList->length; ++i) {
    *exprI = *getExpr(exprList, i);
    if (exprI->label == ATOM || exprI->label == INT || exprI->label == FLOAT || exprI->label == STRING) {
      lineValue = execute(exprI, *currentStack);
    } else if (exprI->label == SEXPR) {
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
      } else if (strcmp(first->atom, ":") == 0) {
        if (getExpr(exprI, 1)->label != ATOM) {
          if (getExpr(exprI, 1)->label == SEXPR) {
            if (strcmp(getExpr(getExpr(exprI, 1), 0)->atom, "get") == 0) {
              char *targetArrayName = getExpr(getExpr(exprI, 1), 1)->atom;
              value targetArray = getVariable(*currentStack, targetArrayName)->Var.Value;
              value n = execute(getExpr(getExpr(exprI, 1), 2), *currentStack);
              if (targetArray.type == ARRAY) {
                *(targetArray.Array + n.Gen.Integer) = execute(getExpr(exprI, 2), *currentStack);
              } else if (targetArray.type == STRING) {
                *(targetArray.Gen.String + n.Gen.Integer) = *(execute(getExpr(exprI, 2), *currentStack).Gen.String);
              }
            } else {
              printf("Error: invalid syntax in assignment.\n:");
              exit(-1);
            }
          } else {
            printf("Error: invalid syntax in assignment.\n:");
            exit(-1);
          }
        } else {
          char *targetVar = getExpr(exprI, 1)->atom;
          getVariable(*currentStack, targetVar)->Var.Value = execute(getExpr(exprI, 2), *currentStack);
        }
        lineValue = initValue();
      } else if (strcmp(first->atom, "function") == 0) {
        variable newFunction = initVariable();
        expr *declaration = getExpr(exprI, 1);
        if (getExpr(declaration, 0)->label != ATOM) {
          printf("Syntax error: function name cannot be %s.\n", getExpr(declaration, 1)->atom);
          exit(-1);
        }
        newFunction.name = getExpr(declaration, 0)->atom;
        newFunction.Var.Function.argumentNames = (char **) calloc(declaration->length - 1, sizeof(char *));
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
      } else if (strcmp(first->atom, "if") == 0) {
        if (execute(getExpr(exprI, 1), *currentStack).Gen.Integer != 0 || execute(getExpr(exprI, 1), *currentStack).Gen.Float != 0.0) {
          expr iftrue = initExpr(SEXPR);
          appendExpr(&iftrue, *getExpr(exprI, 2));
          lineValue = execExprList(&iftrue, NULL, 0, *currentStack, NULL);
        } else {
          if (exprI->length == 4) {
            expr iffalse = initExpr(SEXPR);
            appendExpr(&iffalse, *getExpr(exprI, 3));
            lineValue = execExprList(&iffalse, NULL, 0, *currentStack, NULL);
          }
        }
        lineValue = initValue();
      } else if (strcmp(first->atom, "while") == 0) {
        expr iftrue = initExpr(SEXPR);
        appendExpr(&iftrue, *getExpr(exprI, 2));
        while (execute(getExpr(exprI, 1), *currentStack).Gen.Integer != 0 || execute(getExpr(exprI, 1), *currentStack).Gen.Float != 0.0) {
          lineValue = execExprList(&iftrue, NULL, 0, *currentStack, NULL);
        }
      } else if (strcmp(first->atom, "do") == 0) {
        expr toDo = initExpr(SEXPR);
        for (int j = 1; j < exprI->length; ++j) {
          appendExpr(&toDo, *getExpr(exprI, j));
        }
        lineValue = execExprList(&toDo, NULL, 0, *currentStack, NULL);
      } else if (strcmp(first->atom, "library") == 0) {
        FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
        if (!stream) {
          printf("library: Error: no such file.\n");
          exit(-1);
        }
        char *buffer = NULL;
        size_t len = 0;
        ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
        printf("library: Bytes read: %zd.\n", bytes_read);
        int end = 0;
        expr libModule = fullParse(buffer, strlen(buffer), MODULE, &end);
        free(buffer);
        scopes mainScopes = initScopes();
        map *libSymbols = (map *) calloc(1, sizeof(map));
        execExprList(&libModule, NULL, 0, mainScopes, libSymbols);
        variable thisLibrary = initVariable();
        int libNameLength = getPrefix(getExpr(exprI, 1)->atom); // Gets everything before any file extension
        if (libNameLength == 0) {
          libNameLength = strlen(getExpr(exprI, 1)->atom);
        }
        if (exprI->length == 3) { // Allows declaration of a library with alternate name.
          char *libAsName = getExpr(exprI, 2)->atom;
          thisLibrary.name = (char *) calloc(strlen(libAsName) + 1, sizeof(char));
          memcpy(thisLibrary.name, libAsName, strlen(libAsName));
        } else {
          thisLibrary.name = (char *) calloc(libNameLength + 1, sizeof(char));
          memcpy(thisLibrary.name, getExpr(exprI, 1)->atom, libNameLength);
        }
        printf("Library name: %s.\n", thisLibrary.name);
        thisLibrary.Var.Library = libSymbols;
        newVariable(currentStack->stack, thisLibrary);
      } else if (strcmp(first->atom, "include") == 0) {
        FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
        if (!stream) {
          printf("include: Error: no such file.\n");
          exit(-1);
        }
        char *buffer = NULL;
        size_t len = 0;
        ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
        printf("include: Bytes read: %zd.\n", bytes_read);
        int end = 0;
        expr header = fullParse(buffer, strlen(buffer), MODULE, &end);
        free(buffer);
        scopes mainScopes = initScopes();
        map *headerSymbols = (map *) calloc(1, sizeof(map));
        execExprList(&header, NULL, 0, mainScopes, headerSymbols);
        for (int j = 0; j < headerSymbols->totalBuckets; ++j) {
          bucket thisBucket = *(headerSymbols->Buckets + j);
          for (int k = 0; k < thisBucket.length; ++k) {
            variable newItem = *(thisBucket.contents + k);
            newItem.name = (char *) calloc(strlen(getExpr(exprI, 1)->atom) + 2 + strlen((thisBucket.contents + k)->name), sizeof(char));
            sprintf(newItem.name, "%s", (thisBucket.contents + k)->name);
            newVariable(currentStack->stack, newItem);
          }
        }
      } else if (strcmp(first->atom, "macros") == 0) {
        FILE *stream = fopen(getExpr(exprI, 1)->atom, "r");
        if (!stream) {
          printf("macros: Error: no such file.");
          exit(-1);
        }
        char *buffer = NULL;
        size_t len = 0;
        ssize_t bytes_read = getdelim(&buffer, &len, -1, stream);
        printf("macros: Bytes read: %zd.\n", bytes_read);
        macroArray *thisMacroArray = (macroArray *) calloc(1, sizeof(macroArray));
        int end = 0;
        parse(buffer, strlen(buffer), MODULE, &end, thisMacroArray);
        postParser(exprList, *thisMacroArray);
      } else {
        lineValue = execute(exprI, *currentStack);
      }
    }
  }
  if (ExportTo && exprList->label == MODULE) {
    *ExportTo = initMap(6);
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












