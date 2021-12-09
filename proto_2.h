#include <stdint.h>

// Labels for types of expressions. List types are multiples of 10, so that 
// whether something is a list or an atom can be tested by using the modulus function.
#define ATOM 1
#define LIST 10
#define INT 2
#define FLOAT 3
#define STRING 4
#define SEXPR 20
#define MODULE 30
#define FUNCTION 40
#define ARRAY 50
#define COMMENT 99
#define LIBRARY 5
#define MACRO 60
#define ADDRESS 6
#define SYS_FILE 7

// A structure for storing expressions and expression trees.
typedef struct expr {
  int label; // What kind of expression is this (list or atom)?
  int length; // The number of nested expressions this list contains (if it is a list).
  char *atom; // The contents of the atom.
  struct expr *sub; // The expressions within this list.
} expr;

expr initExpr(int);

expr *getExpr(expr*, int);

typedef struct macroArray macroArray; // Forward declaration
expr parse(char*, int, int, int*, macroArray*);

expr fullParse(char*, int, int, int*);

void appendExpr(expr*, expr);

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

typedef struct map map; // Forward declaration

union thing {
  value Value;
  function Function;
  map *Library; // A pointer is used here since the type is incomplete
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





