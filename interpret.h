#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdio.h>
#include <stdint.h>
#include "parse.h"

union generic {
  int Integer;
  long LongInt;
  double Float;
  char *String;
  FILE *filestream;
};

// Value structure for general use.
typedef struct value {
  int type;
  union generic Gen;
  int arrayLength;
  struct value *Array;
} value;

value initValue(void);

// Function type containing the function body,
// Names of the arguments, and the number of arguments.
typedef struct function {
  expr body;
  char **argumentNames;
  int argumentNumber;
} function;

typedef struct map map;

union thing {
  value Value;
  function Function;
  map *Library;
};

// Variable structure, containing a name, hash,
// and either a value or function.
// Libraries are also stored with this structure.
typedef struct variable {
  char *name;
  uint32_t hash;
  union thing Var;
} variable;

// Bucket structure for use in maps. 
typedef struct bucket {
  int length;
  variable *contents;
} bucket;

// Hash table structure for storing variables.
typedef struct map {
  int hashLength;
  int totalBuckets;
  int usedBuckets;
  bucket *Buckets;
} map;

// Structure for storing maps for the variables in each scope.
typedef struct scopes {
  int depth;
  map *stack;
} scopes;

scopes initScopes(void);

value execExprList(expr*, variable*, int, scopes, map*);

#endif