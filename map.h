#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdio.h>

extern const uint32_t MAP_INIT_HASHLEN;

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
typedef struct {
    expr body;
    char **argumentNames;
    int argumentNumber;
} function;

typedef struct map map;

typedef union {
    value Value;
    function Function;
    map *Library;
} thing;

// Variable structure, containing a name, hash,
// and either a value or function.
// Libraries are also stored with this structure.
typedef struct {
    char *name;
    uint32_t hash;
    thing Var;
} variable;

variable initVariable(void);

// Bucket structure for use in maps.
typedef struct {
    int length;
    variable *contents;
} bucket;

// Hash table structure for storing variables.
struct map {
    int hashLength;
    int totalBuckets;
    int usedBuckets;
    bucket *Buckets;
};

map initMap(void);

// Structure for storing maps for the variables in each scope.
typedef struct {
    int depth;
    map *stack;
} scopes;

scopes initScopes(void);

void delMapContents(map);

variable *mapGet(map *, char *);

void newVariable(map *, variable);

variable *getVariable(scopes, char *);

scopes plusScope(scopes, map);

#endif